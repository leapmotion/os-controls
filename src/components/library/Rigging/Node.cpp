//
//  Node.cpp
//  AssimpApp
//
//  Created by Éric Renaud-Houde on 2013-02-22.
//
//

#include "Node.h"

namespace model {
	
	Node::Node( const Eigen::Vector3f& position, const Eigen::Quaternionf& rotation, const Eigen::Vector3f& scale,
			    const std::string& name, NodeRef parent, int level )
	: mInitialRelativePosition( position )
	, mInitialRelativeRotation( rotation )
	, mInitialRelativeScale( scale )
	, mRelativePosition( position )
	, mRelativeRotation( rotation )
	, mRelativeScale( scale )
  , mAbsolutePosition( Eigen::Vector3f::Zero() )
  , mAbsoluteScale( Eigen::Vector3f::Zero() )
  , mAbsoluteRotation( Eigen::Quaternionf::Identity())
	, mName( name )
	, mParent( parent )
	, mLevel( level )
	, mBoneIndex( -1 )
	, mTime( 0.0f )
	, mIsAnimated( false )
	, mNeedsUpdate( true )
	{
	}
	
	NodeRef Node::clone() const
	{
		NodeRef clone = NodeRef( new Node(mRelativePosition,
										  mRelativeRotation,
										  mRelativeScale,
										  mName,
										  nullptr,
										  mLevel ) );
		if (mOffset)
			clone->setOffsetMatrix( *mOffset );
		clone->setBoneIndex( getBoneIndex() );
		clone->mBoneIndex = mBoneIndex;
		
		//TODO: Copy animation data.
		//	clone->mIsAnimated = mIsAnimated;
		clone->animate( mTime );
		return clone;
	}
	
	void Node::computeTransformation( const Eigen::Vector3f& t, const Eigen::Quaternionf& r, const Eigen::Vector3f& s,  Eigen::Matrix4f* transformation )
	{
    Eigen::Matrix4f& trans = *transformation;
    trans.setIdentity();
    trans.block<3, 3>(0, 0) = s.asDiagonal();
    Eigen::Matrix4f rotMatrix(Eigen::Matrix4f::Identity());
    rotMatrix.block<3, 3>(0, 0) = r.matrix();
    trans = rotMatrix * trans;
    trans.block<3, 1>(0, 3) = t;
	}
	
	void Node::setParent( NodeRef parent )
	{
		mParent = parent;
		requestSubtreeUpdate();
	}
	
	Eigen::Matrix4f Node::getRelativeTransformation()
	{
		Eigen::Matrix4f transformation;
		computeTransformation( mRelativePosition, mRelativeRotation, mRelativeScale, &transformation );
		return transformation;
	}
	
	Eigen::Matrix4f Node::getAbsoluteTransformation()
	{
		if( mNeedsUpdate ) {
			update();
		}
		Eigen::Matrix4f transformation;
		computeTransformation( mAbsolutePosition, mAbsoluteRotation, mAbsoluteScale, &transformation );
		return transformation;
	}
	
	Eigen::Vector3f& Node::getAbsolutePosition()
	{
		if( mNeedsUpdate ) {
			update();
		}
		return mAbsolutePosition;
	}
	
	const Eigen::Vector3f& Node::getAbsolutePosition() const
	{
		if( mNeedsUpdate ) {
			update();
		}
		return mAbsolutePosition;
	}
	
	Eigen::Quaternionf& Node::getAbsoluteRotation()
	{
		if( mNeedsUpdate ) {
			update();
		}
		return mAbsoluteRotation;
	}
	
	const Eigen::Quaternionf& Node::getAbsoluteRotation() const
	{
		if( mNeedsUpdate ) {
			update();
		}
		return mAbsoluteRotation;
	}
	
	Eigen::Vector3f& Node::getAbsoluteScale()
	{
		if( mNeedsUpdate ) {
			update();
		}
		return mAbsoluteScale;
	}
	
	const Eigen::Vector3f& Node::getAbsoluteScale() const
	{
		if( mNeedsUpdate ) {
			update();
		}
		return mAbsoluteScale;
	}

	void Node::setAbsolutePosition( const Eigen::Vector3f& pos )
	{
		mAbsolutePosition = pos;
		std::shared_ptr<Node> parent( mParent.lock() );
		if ( parent ) {
			setRelativePosition( mAbsolutePosition - parent->getAbsolutePosition() );
		} else {
			setRelativePosition( mAbsolutePosition );
		}
	}
	
	void Node::setRelativePosition( const Eigen::Vector3f& pos )
	{
		mRelativePosition = pos;
		requestSubtreeUpdate();
	}
	
	void Node::setRelativeRotation( const Eigen::Quaternionf& rotation )
	{
		mRelativeRotation = rotation;
		requestSubtreeUpdate();
	}
	
	void Node::setRelativeScale( const Eigen::Vector3f& scale )
	{
		mRelativeScale = scale;
		requestSubtreeUpdate();
	}
	
	void Node::resetToInitial()
	{
		mRelativePosition = mInitialRelativePosition;
		mRelativeRotation = mInitialRelativeRotation;
		mRelativeScale = mInitialRelativeScale;
	}
	
	void Node::addChild( NodeRef node )
	{
		mChildren.push_back( node );
	}

	void Node::update() const
	{
		// update orientation
		std::shared_ptr<Node> parent( mParent.lock() );
		if ( parent ) {
			const Eigen::Quaternionf& parentRotation = parent->getAbsoluteRotation();
			const Eigen::Vector3f& parentScale = parent->getAbsoluteScale();

      mAbsoluteRotation = parentRotation * mRelativeRotation;
			mAbsoluteScale = mRelativeScale.cwiseProduct(parentScale);
			
			// change position vector based on parent's rotation & scale
      mAbsolutePosition = parentRotation * (parentScale.cwiseProduct(mRelativePosition));

			// add altered position vector to parent's
			mAbsolutePosition += parent->getAbsolutePosition();
		} else {
			mAbsoluteRotation = mRelativeRotation;
			mAbsoluteScale = mRelativeScale;
		}
		mNeedsUpdate = false;
	}
	
	void Node::animate( float time, int trackId )
	{
		mTime = time;
		mIsAnimated = false;
		requestSubtreeUpdate();
		update();
	}

	void Node::requestSubtreeUpdate()
	{
		mNeedsUpdate = true;
		
		for( auto childNode : mChildren ) {
			/* 
			 * We can prune out subsections of the tree which already need updates
			 * because a node that already needs updating will have traversed all its
			 * its children. Since all updates are usually done all at once, we don't
			 * retraverse the entire subtrees everytime.
			 */
			if( !childNode->mNeedsUpdate ) {
				childNode->requestSubtreeUpdate();
			}
		}
	}
	
} //end namespace model

