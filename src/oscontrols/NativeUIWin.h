#pragma once
#include "NativeUI.h"

namespace oscontrols {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
  using namespace System::Resources;
  using namespace System::Reflection;

	/// <summary>
	/// Summary for NativeUI
	/// </summary>
	public ref class NativeUIWin : public System::Windows::Forms::Form
	{
	public:
    NativeUIWin(const NativeCallbacks& callbacks) :
      callbacks(callbacks)
		{
			InitializeComponent();
			
      ResourceManager^ rm = gcnew ResourceManager("oscontrols.Resource", Assembly::GetExecutingAssembly());
      System::Drawing::Bitmap^ bmp = (System::Drawing::Bitmap^) rm->GetObject("icon_512x512");

      auto icon = System::Drawing::Icon::FromHandle(bmp->GetHicon());
      this->notificationIcon->Icon = icon;
      this->Icon = icon;
		}

    static size_t s_nativeUIInitCount = 0;
    static NativeUIWin^ s_nativeUI;

    static void AddTrayIcon(const NativeCallbacks& callbacks) {
      if(!s_nativeUIInitCount++)
        s_nativeUI = gcnew NativeUIWin(callbacks);
    }

    static void RemoveTrayIcon(void) {
      if(--s_nativeUIInitCount)
        return;

      s_nativeUI->Close();
      s_nativeUI = nullptr;
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

    const NativeCallbacks& callbacks;

  private: System::Windows::Forms::NotifyIcon^  notificationIcon;
  private: System::Windows::Forms::ContextMenuStrip^  notificationMenu;



  protected:

  protected:

  private: System::Windows::Forms::ToolStripMenuItem^  configToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;


  protected:
  private: System::ComponentModel::IContainer^  components;

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
      this->configToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->notificationMenu->SuspendLayout();
      this->SuspendLayout();
      // 
      // notificationIcon
      // 
      this->notificationIcon->ContextMenuStrip = this->notificationMenu;
      this->notificationIcon->Text = L"Leap Hand Control";
      this->notificationIcon->Visible = true;
      // 
      // notificationMenu
      // 
      this->notificationMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
        this->configToolStripMenuItem,
          this->exitToolStripMenuItem
      });
      this->notificationMenu->Name = L"contextMenuStrip1";
      this->notificationMenu->ShowImageMargin = false;
      this->notificationMenu->Size = System::Drawing::Size(133, 48);
      // 
      // configToolStripMenuItem
      // 
      this->configToolStripMenuItem->Name = L"configToolStripMenuItem";
      this->configToolStripMenuItem->Size = System::Drawing::Size(132, 22);
      this->configToolStripMenuItem->Text = L"&Configuration...";
      this->configToolStripMenuItem->Click += gcnew System::EventHandler(this, &NativeUIWin::configToolStripMenuItem_Click);
      // 
      // exitToolStripMenuItem
      // 
      this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
      this->exitToolStripMenuItem->Size = System::Drawing::Size(132, 22);
      this->exitToolStripMenuItem->Text = L"&Quit";
      this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &NativeUIWin::exitToolStripMenuItem_Click);
      // 
      // NativeUIWin
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(284, 262);
      this->MaximizeBox = false;
      this->MinimizeBox = false;
      this->Name = L"NativeUIWin";
      this->Text = L"Configuration";
      this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &NativeUIWin::NativeUIWin_FormClosing);
      this->notificationMenu->ResumeLayout(false);
      this->ResumeLayout(false);

    }
#pragma endregion
  void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
    callbacks.OnQuit();
    Close();
  }
  void configToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
    Visible = true;
  }
  void NativeUIWin_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
    e->Cancel = true;
    Visible = false;
  }
};
}
