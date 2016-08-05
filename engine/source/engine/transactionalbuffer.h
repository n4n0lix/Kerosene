#pragma once

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                        Includes                        */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// Std-Includes


// Internal Includes
#include "_global.h"

#include "buffer.h"
#include "buffertoken.h"

#include "tb_writeop.h"
#include "tb_removeop.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                         Class                          */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
ENGINE_NAMESPACE_BEGIN

template<class T>
class TransactionalBuffer : public Buffer<T>
{
public:
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                        Public                          */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                                                TransactionalBuffer(uint32 objSize, uint32 objCapacity);


            void                                commit();

    virtual uint32                              num_objects();

    virtual shared_ptr<BufferToken>             write(Vector<T> objects);
    virtual void                                remove(shared_ptr<BufferToken> token);
protected:
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                       Protected                        */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    const   Vector<shared_ptr<BufferToken>>&    get_active_tokens();
    virtual shared_ptr<BufferToken>             create_token(uint32 tokenId); // Override this if custom token class is used

    // Inter-Implementation
    virtual void                                commit_write(Vector<T> objects, shared_ptr<BufferToken> commit_token) = 0;
    virtual void                                commit_remove(shared_ptr<BufferToken> token) = 0;

private:
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                        Private                         */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            uint32                              generate_token_id();
            uint32                              generate_writeop_id();
            uint32                              generate_removeop_id();

    uint32                                  _numObjects;

    Vector<shared_ptr<TB_WriteOp<T>>>       _writeBucket;
    Vector<shared_ptr<BufferToken>>         _tokenToRemove;
    Vector<shared_ptr<BufferToken>>         _activeTokens;

    uint32                  _nextUidToken;
    uint32                  _nextUidWriteOp;
    uint32                  _nextUidRemoveOp;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*                     Private Static                     */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    static Logger LOGGER;
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                        Public                          */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

template<class T>
TransactionalBuffer<T>::TransactionalBuffer(uint32 objSize, uint32 objCapacity) : Buffer(objSize, objCapacity)
{
    _nextUidToken = 0;
    _nextUidWriteOp = 0;
    _nextUidRemoveOp = 0;
    _numObjects = 0;
}

template<class T>
shared_ptr<BufferToken> TransactionalBuffer<T>::write(Vector<T> objects)
{
    // 1# Guards
    Requires( !objects.empty() );

    // 2# Create token
    shared_ptr<BufferToken> token = create_token(generate_token_id());

    // 3# Store in write bucket
    _writeBucket.add(make_shared<TB_WriteOp<T>>(generate_writeop_id(), token, std::move(objects)));
    _activeTokens.add(token);

    // 4# Return token
    return token;
}

template<class T>
void TransactionalBuffer<T>::remove(shared_ptr<BufferToken> token)
{
    // 0# Contract pre
    Requires( token != nullptr );

    // 1# Check if token is invalid, and if it is remove corresponding writeops ...
    if (!token->valid()) {
        _writeBucket.remove([&](shared_ptr<TB_WriteOp<T>> op) -> bool { return op->token() == token; });
        return;
    }

    // 2# Create RemoveOp for token
    _tokenToRemove.add( token );
}

template<class T>
shared_ptr<BufferToken> TransactionalBuffer<T>::create_token(uint32 tokenId)
{
    return make_shared<BufferToken>(tokenId, this);
}

template<class T>
void TransactionalBuffer<T>::commit() {
    // 1# Do RemoveOps
    for (auto removeToken : _tokenToRemove) {
        commit_remove(removeToken);
        _numObjects -= removeToken->object_range().length();
    }
    _tokenToRemove.clear();

    // 2# Do WriteOps
    for (auto writeOp : _writeBucket) {
        auto writeToken = writeOp->token();
        auto objects = writeOp->claim_objects();
        commit_write( std::move( objects ), writeToken );
        _numObjects += writeToken->object_range().length();
    }
    _writeBucket.clear();

    // X# Contract Post
    Ensures( _tokenToRemove.empty() );
    Ensures( _writeBucket.empty() );
}

template<class T>
uint32 TransactionalBuffer<T>::num_objects()
{
    return _numObjects;
}

template<class T>
const Vector<shared_ptr<BufferToken>>& TransactionalBuffer<T>::get_active_tokens()
{
    return _activeTokens;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                        Private                         */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

template<class T>
uint32 TransactionalBuffer<T>::generate_token_id() {
    return _nextUidToken++;
}

template<class T>
uint32 TransactionalBuffer<T>::generate_writeop_id() {
    return _nextUidWriteOp++;
}

template<class T>
uint32 TransactionalBuffer<T>::generate_removeop_id() {
    return _nextUidRemoveOp++;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*                     Private Static                     */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

template<class OBJECT>
Logger TransactionalBuffer<OBJECT>::LOGGER = Logger("TransactionalBuffer<>", Level::DEBUG);

ENGINE_NAMESPACE_END