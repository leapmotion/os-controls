#pragma once

struct GestureTriggerManifestState {
  bool m_foo;
};

struct GestureTriggerManifest
{
public:
  GestureTriggerManifest();

  void AutoFilter(Leap::Frame frame, const HandExistenceState& state, GestureTriggerManifestState& gtmfState);

  Autowired<HandExistTrigger> m_hat;
};

