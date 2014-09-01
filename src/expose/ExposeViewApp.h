#pragma once

class ExposeViewApp:
  ExposeViewApp::Activation
{
public:
  ExposeViewApp(void);
  ~ExposeViewApp(void);

private:
  // Activation overrides:
  void SetActivation(float activation) override;

public:

  std::shared_ptr<Activation> ObtainActivation(void);
};

