#pragma once
#include "graphics/Wedges.h"
#include "graphics/RenderEngine.h"
#include "graphics/VolumeControl.h"
#include "interaction/MediaViewController.h"
#include "interaction/HandRollRecognizer.h"
#include "uievents/HandProperties.h"
#include "uievents/MediaViewEventListener.h"
#include "SceneGraphNode.h"
#include "Leap.h"

#include "Primitives.h"
#include "Animation.h"
#include <array>

enum class HandPose;
enum class OSCState;
struct HandLocation;

class MediaView :
  public RenderEngineNode
{
public:
  MediaView(const Vector3& center, float offset);
  ~MediaView();
  
  void AutoInit();

  //Non-User-Input driven animation and rendering update (eg. fade-in and fade-out)
  void AnimationUpdate(const RenderFrame& frame) override;
  
  //Set the translation portion of our node's tranform.
  void Move(const Vector3& coords);

  void OpenMenu(const HandLocation& handLocation);
  void CloseMenu();
  
  //Accessors
  bool IsVisible() const;
  float GetDistanceFromCenter(const HandLocation& handLocation) const;
  std::shared_ptr<Wedge> GetActiveWedgeFromHandLocation(const HandLocation& handLocation) const;
  bool checkForSelection(std::shared_ptr<Wedge> activeWedge, float distanceFromDeadzone) const;
  
  //Updators
  void UpdateWedges(std::shared_ptr<Wedge> activeWedge, float distanceFromDeadzone);
  void SetVolumeView(float volume);
  void NudgeVolumeView(float dVolume);
  
  // MediaView properties
  Animated<float> m_opacity;
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  //Wrappers on some fading in and out logic.
  void fadeIn() { m_opacity.Set(1.0f); }
  void fadeOut() { m_opacity.Set(0.0f); }
  
  //Send direction about max opacity and goal opacity to the individual wedges.
  void setMenuOpacity(float opacity);

  //TODO: Get rid of magic number (which is the number of wedges)
  std::array<std::shared_ptr<Wedge>, 4> m_wedges;
  std::shared_ptr<VolumeControl> m_volumeControl;
  
  Autowired<RootRenderEngineNode> m_rootNode;
  
  
};