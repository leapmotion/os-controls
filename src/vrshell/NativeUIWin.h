#pragma once
#include "NativeUI.h"

namespace VRShell {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
  using namespace System::Resources;
  using namespace System::Reflection;

	/// <summary>
	/// Summary for NativeUIWin
	/// </summary>
	public ref class NativeUIWin : public System::Windows::Forms::Form
	{
	public:
		NativeUIWin(NativeUI& callbacks):
      m_callbacks(callbacks)
		{
			InitializeComponent();

      ResourceManager^ rm = gcnew ResourceManager("VRShell.Resources", Assembly::GetExecutingAssembly());
      System::Drawing::Bitmap^ appBmp = (System::Drawing::Bitmap^) rm->GetObject("icon_512x512");
      this->Icon = System::Drawing::Icon::FromHandle(appBmp->GetHicon());

      System::Drawing::Icon^ trayIcon = (System::Drawing::Icon^) rm->GetObject("icon");
      this->notificationIcon->Icon = trayIcon;
		}

    static size_t s_nativeUIInitCount = 0;
    NativeUI& m_callbacks;
    private: System::Windows::Forms::NotifyIcon^  notificationIcon;
    private: System::Windows::Forms::ContextMenuStrip^  notificationMenu;
    private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;


  public:
    static NativeUIWin^ s_nativeUI;

    static void AddTrayIcon(NativeUI& callbacks) {
      if (s_nativeUIInitCount++)

      s_nativeUIInitCount++;
      s_nativeUI = gcnew NativeUIWin(callbacks);
    }
    static void RemoveTrayIcon(void) {
      if (!--s_nativeUIInitCount)
        return;

      s_nativeUI->Close();
      s_nativeUI = nullptr;
    }
    static void ShowToolbarMessage(const char* title, const char* message) {
      if (!s_nativeUI)
        return;

      s_nativeUI->notificationIcon->ShowBalloonTip(10000, gcnew String(title), gcnew String(message), Windows::Forms::ToolTipIcon::Warning);
    }
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~NativeUIWin()
		{
			if (components)
			{
				delete components;
			}
		}
  private: System::ComponentModel::IContainer^  components;
  protected:

   

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
      this->components = (gcnew System::ComponentModel::Container());
      this->notificationIcon = (gcnew System::Windows::Forms::NotifyIcon(this->components));
      this->notificationMenu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
      this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->notificationMenu->SuspendLayout();
      this->SuspendLayout();
      // 
      // notificationIcon
      // 
      this->notificationIcon->Text = L"Quick Switch";
      this->notificationIcon->Visible = true;
      // 
      // notificationMenu
      // 
      this->notificationMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->exitToolStripMenuItem });
      this->notificationMenu->Name = L"notificationMenu";
      this->notificationMenu->Size = System::Drawing::Size(93, 26);
      this->notificationMenu->Text = L"Exit";
      // 
      // exitToolStripMenuItem
      // 
      this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
      this->exitToolStripMenuItem->Size = System::Drawing::Size(92, 22);
      this->exitToolStripMenuItem->Text = L"&Exit";
      this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &NativeUIWin::exitToolStripMenuItem_Click);
      // 
      // NativeUIWin
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(284, 261);
      this->Name = L"NativeUIWin";
      this->Text = L"NativeUIWin";
      this->notificationMenu->ResumeLayout(false);
      this->ResumeLayout(false);

    }
#pragma endregion
  private: System::Void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
    m_callbacks.OnQuit();
    Close();
  }
};
}
