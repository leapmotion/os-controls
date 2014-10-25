//
//  Node.h
//  AssimpApp
//
//  Created by Éric Renaud-Houde on 2013-02-22.
//
//

#pragma once

#include "EigenTypes.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace model {

typedef std::shared_ptr<class Node> NodeRef;

class Node {
public:
	enum RenderMode { CONNECTED, JOINTS };
	
	explicit Node( const Eigen::Vector3f& position, const Eigen::Quaternionf& rotation, const Eigen::Vector3f& scale,
				   const std::string& name, NodeRef parent = nullptr, int level = 0 );
	
	NodeRef clone() const;
	
	void addChild( NodeRef Node );
	
	const std::weak_ptr<Node>&		getParent() const { return mParent; }
	std::weak_ptr<Node>&			getParent() { return mParent; }
	void				setParent( NodeRef parent );
	
	int		getNumChildren() const { return mChildren.size(); }
	bool	hasChildren() const { return !mChildren.empty(); }
	bool	hasParent()   const { return !mParent.expired(); }
	
	const std::vector<NodeRef>& getChildren() const { return mChildren; }
	std::vector<NodeRef>&		getChildren() { return mChildren; }
	
	const std::string&	getName() const { return mName; }
	void				setName( const std::string& name ) { mName = name; }
	
	Eigen::Vector3f&				getRelativePosition() { return mRelativePosition; }
	const Eigen::Vector3f&		getRelativePosition() const { return mRelativePosition; }
	Eigen::Quaternionf&				getRelativeRotation() { return mRelativeRotation; }
	const Eigen::Quaternionf&		getRelativeRotation() const { return mRelativeRotation; }
	Eigen::Vector3f&				getRelativeScale() { return mRelativeScale; }
	const Eigen::Vector3f&		getRelativeScale() const { return mRelativeScale; }
	Eigen::Vector3f&				getInitialRelativePosition() { return mInitialRelativePosition; }
	const Eigen::Vector3f&		getInitialRelativePosition() const { return mInitialRelativePosition; }
	Eigen::Quaternionf&				getInitialRelativeRotation() { return mInitialRelativeRotation; }
	const Eigen::Quaternionf&		getInitialRelativeRotation() const { return mInitialRelativeRotation; }
	Eigen::Vector3f&				getInitialRelativeScale() { return mInitialRelativeScale; }
	const Eigen::Vector3f&		getInitialRelativeScale() const { return mInitialRelativeScale; }
	
	Eigen::Matrix4f			getRelativeTransformation();
	Eigen::Matrix4f			getAbsoluteTransformation();
	
	Eigen::Vector3f&				getAbsolutePosition();
	const Eigen::Vector3f&		getAbsolutePosition() const;
	Eigen::Quaternionf&				getAbsoluteRotation();
	const Eigen::Quaternionf&		getAbsoluteRotation() const;
	Eigen::Vector3f&				getAbsoluteScale();
	const Eigen::Vector3f&		getAbsoluteScale() const;
	
	// Setting absolute positions is a more expensive operation: we have to compute relative ones
	void					setAbsolutePosition( const Eigen::Vector3f& pos );

	void					setRelativePosition( const Eigen::Vector3f& pos );
	void					setRelativeRotation( const Eigen::Quaternionf& rotation );
	void					setRelativeScale( const Eigen::Vector3f& scale );
	
	void					resetToInitial();
	
	int		getLevel() const { return mLevel; }
	void	setLevel( int level ) { mLevel = level; }
	int		getBoneIndex() const { return mBoneIndex; }
	void	setBoneIndex( int boneIndex ) { mBoneIndex = boneIndex; }
	
	bool	isAnimated() const { return mIsAnimated; }
	float	getTime() { return mTime; }
	
	/*! 
	 *  Update the relative and absolute transformations using animation curves (if animated).
	 *	If the node is not animated, its absolute transformation is still updated based on its
	 *  parent because that in turn may be animated. No traversal is done.
	 */
	void	animate( float time, int trackId = 0 );
	
	void		setOffsetMatrix( const Eigen::Matrix4f& offset ) { mOffset =  std::unique_ptr<Eigen::Matrix4f>( new Eigen::Matrix4f(offset) ); }
	const std::unique_ptr<Eigen::Matrix4f>& getOffset() { return mOffset; }
	
	bool operator==( Node &rhs )
	{
		return (	getName() == rhs.getName()
				&& getLevel() == rhs.getLevel() );
	}
	
	bool operator!=( Node &rhs )
	{
		return !(*this == rhs);
	}

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

protected:
	
	static void computeTransformation( const Eigen::Vector3f& t, const Eigen::Quaternionf& r, const Eigen::Vector3f& s,  Eigen::Matrix4f* transformation );

	void	update() const;
	void	requestSubtreeUpdate();
	bool	hasAnimations( int trackId = 0 ) const;
	
	mutable bool		mNeedsUpdate;
	Eigen::Vector3f			mRelativePosition, mRelativeScale;
	Eigen::Quaternionf			mRelativeRotation;
	mutable Eigen::Vector3f	mAbsolutePosition, mAbsoluteScale;
	mutable Eigen::Quaternionf	mAbsoluteRotation;
	float		mTime;
	/*! A boolean indicating whether a node is animated.
	 * We are using a dynamic boolean flag because given a certain combination
	 * of animation blends, different nodes may become animated during runtime. 
	 */
	mutable bool			mIsAnimated;
	
	mutable Eigen::Vector3f mInitialRelativePosition;
	mutable Eigen::Quaternionf mInitialRelativeRotation;
	mutable Eigen::Vector3f mInitialRelativeScale;
	
	std::unique_ptr<Eigen::Matrix4f> mOffset;
	
	std::weak_ptr<Node> mParent;
	std::vector<NodeRef> mChildren;
	std::string	mName;
	int			mLevel;
	int			mBoneIndex;
	
	/*!
	 * An unordered_map storing the different animation tracks with
	 * their specific int trackId key.
	 * This data-structure was chosen because a bone may be
	 * animated in some animation tracks, and not in others.
	 * */
	//std::unordered_map< int, std::shared_ptr<AnimTrack> >	mAnimTracks;
private:
	Node( const Node &rhs ); // private to prevent copying; use clone() method instead
	Node& operator=( const Node &rhs ); // not defined to prevent copying
};
	
} //end namespace model
