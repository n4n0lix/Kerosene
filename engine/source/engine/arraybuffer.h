#pragma once

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                        Includes                        */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// Std-Includes
#include <stdexcept>
#include <functional>

// Internal Includes
#include "_global.h"
#include "vector.h"

#include "transactionalbuffer.h"
#include "bufferrange.h"

#include "tb_token.h"
#include "tb_writeop.h"
#include "tb_removeop.h"

// TODO: Optimize the methods 'optimizeNext' and 'optimize'
// TODO: Merge RemoveOps and WriteOps to decrease the num of operations
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                         Class                          */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
ENGINE_NAMESPACE_BEGIN

template<class T>
class ArrayBuffer : public TransactionalBuffer<T>
{

public:
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                        Public                          */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                                        ArrayBuffer(uint32 objCapacity, uint32 objSize);

    using                               TransactionalBuffer<T>::commit;
    using                               TransactionalBuffer<T>::write;
    using                               TransactionalBuffer<T>::remove;
    using                               TransactionalBuffer<T>::num_objects;
protected:
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                       Protected                        */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    virtual void                        commit_write(shared_ptr<vector<T>> objects, shared_ptr<TB_Token> token);
    virtual void                        commit_remove(shared_ptr<TB_Token> token);

    // Final-Implementation
    virtual void                        write(uint32 index, shared_ptr<vector<T>> objects) = 0;
    virtual void                        remove(uint32 index, uint32 length) { };
    virtual void                        resize(uint32 oldCapacity, uint32 newCapacity) = 0;

private:
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                        Private                         */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            void                        optimize();
            bool                        optimizeNext();
            void                        mergeAdjacentFreeRanges(BufferRange range1, BufferRange range2);
            BufferRange                 getFreeRange(uint32 length);

            void                        _resize(uint32 oldCapacity, uint32 newCapacity);

            uint32                      generateRangeId();
            uint32                      generateTokenId();

    Vector<BufferRange>         _freeRanges;
    Vector<BufferRange>         _usedRanges;

    uint32                      _nextUidRange;
    uint32                      _nextUidToken;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                     Private Static                     */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    static Logger LOGGER;
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                        Public                          */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

template<class T>
ArrayBuffer<T>::ArrayBuffer(uint32 objSize, uint32 objCapacity) : TransactionalBuffer(objSize, objCapacity)
{
    _nextUidRange    = BufferRange::FIRST_ID;

    // 1# Create initial range
    auto initialRange = BufferRange(generateRangeId(), this, 0, atom_capacity());
    _freeRanges.add(initialRange);
    
    LOGGER.log(Level::DEBUG) << "CREATE [" << initialRange.index() << "," << initialRange.length()-1 << "], OBJ SIZE: " << object_size()  << endl;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                       Protected                        */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

template<class T>
void ArrayBuffer<T>::commit_write(shared_ptr<vector<T>> objects, shared_ptr<TB_Token> token)
{
    // 1# Get free range
    uint32_t size = (uint32_t)objects->size() * object_size();
    BufferRange freeRange = getFreeRange(size);

    _freeRanges.remove(freeRange);
    _usedRanges.add(freeRange);

    // 2#  Write
    write(freeRange.index(), objects);
    LOGGER.log(Level::DEBUG) << "WRITE " << objects->size() << " AT [" << freeRange.index() << ", " << freeRange.index() + size - 1 << "]" << endl;

    // 3# Update and validate the token
    token->set_range_id(freeRange.id());
    token->set_data(freeRange.index(), freeRange.length(), object_size());
    token->validate();
}

template<class T>
void ArrayBuffer<T>::commit_remove(shared_ptr<TB_Token> token)
{
    // 1# Get used range
    uint32 rangeId = token->range_id();
    Vector<BufferRange> ranges;

    _usedRanges.forAll([&](BufferRange& range) -> void {
        if (range.id() == rangeId) {
            ranges.add(range);
        }
    });

    // 2# Remove range
    for (BufferRange range : ranges) {
        remove(range.index(), range.length());
        _usedRanges.remove(range);
        _freeRanges.add(range);
    }

    // 3# Invalidate token
    token->invalidate();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                        Private                         */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

template<class T>
void ArrayBuffer<T>::optimize() {
    bool hasChanged = false;

    do {
        hasChanged = optimizeNext();
    } while (hasChanged);
}

template<class T>
bool ArrayBuffer<T>::optimizeNext() {
    for (size_t i = 0; i < _freeRanges.size(); i++) {
        for (size_t p = i + 1; p < _freeRanges.size(); p++) {
            auto range1 = _freeRanges.at(i);
            auto range2 = _freeRanges.at(p);

            if (range1.index() + range1.length() == range2.index()) {
                mergeAdjacentFreeRanges(range1, range2);
                return true;
            }
            else if (range2.index() + range2.length() == range1.index()) {
                mergeAdjacentFreeRanges(range2, range1);
                return true;
            }
        }
    }

    return false;
}

template<class T>
void ArrayBuffer<T>::mergeAdjacentFreeRanges(BufferRange range1, BufferRange range2) {
    // 1# Guards
    if (!_freeRanges.contains(range1)) {
        return;
    }

    if (!_freeRanges.contains(range2)) {
        return;
    }

    if (range1.index() + range1.length() != range2.index()) {
        return;
    }

    // 2# Create new range
    auto mergedRange = BufferRange(generateRangeId(), this, range1.index(), range1.length() + range2.length());

    // 3# Swap out ranges
    _freeRanges.remove(range1);
    _freeRanges.remove(range2);
    _freeRanges.add(mergedRange);
}

template<class T>
BufferRange ArrayBuffer<T>::getFreeRange(uint32 length) {
    // 1# Find a range with range.length >= length
    BufferRange freeRange;
    bool foundRange = false;

    for (auto aRange : _freeRanges) {
        if (aRange.length() >= length) {
            freeRange = aRange;
            foundRange = true;
            break;
        }
    }

    // 2.1# If no range found: resize, search again
    if (!foundRange) {
        _resize(atom_capacity(), atom_capacity() + length);

        for (auto aRange : _freeRanges) {
            if (aRange.length() >= length) {
                freeRange = aRange;
                foundRange = true;
                break;
            }
        }

        if (!foundRange) {
            throw std::logic_error("No free range found after resizing! Make sure resizing is implemented properly!");
        }
    }

    // 2.2# If range is larger than needed, split
    if (freeRange.length() > length) {
        auto oldRange = freeRange;
        auto newRange1 = BufferRange(generateRangeId(), this, oldRange.index(), length);

        uint32_t newRange2Index = newRange1.index() + newRange1.length();
        uint32_t newRange2Length = oldRange.length() - length;
        auto newRange2 = BufferRange(generateRangeId(), this, newRange2Index, newRange2Length);

        _freeRanges.remove(oldRange);
        _freeRanges.add(newRange1);
        _freeRanges.add(newRange2);

        freeRange = newRange1;
    }

    // 2.2# Return range
    return freeRange;
}

template<class T>
void ArrayBuffer<T>::_resize(uint32 oldAtomCapacity, uint32 newAtomCapacity) {
    resize(oldAtomCapacity, newAtomCapacity);

    set_atom_capacity(newAtomCapacity);
    _freeRanges.add(BufferRange(generateRangeId(), this, oldAtomCapacity, newAtomCapacity - oldAtomCapacity));

    optimize();
}

template<class T>
uint32 ArrayBuffer<T>::generateRangeId() {
    return _nextUidRange++;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                     Private Static                     */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

template<class OBJECT>
Logger ArrayBuffer<OBJECT>::LOGGER = Logger("ArrayBuffer<>", Level::OFF);

ENGINE_NAMESPACE_END