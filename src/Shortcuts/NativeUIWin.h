#pragma once
#include "NativeUI.h"
#include <msclr/marshal_cppstd.h>

struct NativeUI;

namespace Shortcuts {

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
    NativeUIWin(NativeUI& callbacks):
      callbacks(callbacks)
		{
			InitializeComponent();
			
      ResourceManager^ rm = gcnew ResourceManager("Shortcuts.Resource", Assembly::GetExecutingAssembly());
      System::Drawing::Bitmap^ appBmp = (System::Drawing::Bitmap^) rm->GetObject("icon_512x512");
      this->Icon = System::Drawing::Icon::FromHandle(appBmp->GetHicon());

      System::Drawing::Bitmap^ trayBmp = (System::Drawing::Bitmap^) rm->GetObject("icon_16x16");
      this->notificationIcon->Icon = System::Drawing::Icon::FromHandle(trayBmp->GetHicon());
		}

    static size_t s_nativeUIInitCount = 0;
    NativeUI& callbacks;
  private: System::Windows::Forms::CheckBox^  mediaCheckBox;
  public:

  private: System::Windows::Forms::CheckBox^  exposeCheckBox;
  private: System::Windows::Forms::CheckBox^  scrollCheckBox;
  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
  public:




  public:

  public:
    static NativeUIWin^ s_nativeUI;

    static void AddTrayIcon(NativeUI& callbacks) {
      if (!s_nativeUIInitCount++)
      {
        s_nativeUI = gcnew NativeUIWin(callbacks);

        String^ appData = Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData);
        String^ newDir = System::IO::Path::Combine(appData, L"Leap Motion\\Shortcuts");
        if( !System::IO::Directory::Exists(newDir) )
          System::IO::Directory::CreateDirectory(newDir);

        String^ configFile = System::IO::Path::Combine(newDir, L"config.json");
        msclr::interop::marshal_context ctxt;
        std::string str = ctxt.marshal_as<std::string>(configFile);
        callbacks.SetUserConfigFile(str);
        callbacks.RequestConfigs();
      }
        
    }

    static void RemoveTrayIcon(void) {
      if(--s_nativeUIInitCount)
        return;

      s_nativeUI->Close();
      s_nativeUI = nullptr;
    }

    static void ConfigChanged(const std::string& var, bool value){
      if (!s_nativeUI)
        return;

      if (var == "enableScroll")
        s_nativeUI->scrollCheckBox->Checked = value;
      else if (var == "enableWindowSelection")
        s_nativeUI->exposeCheckBox->Checked = value;
      else if (var == "enableMedia")
        s_nativeUI->mediaCheckBox->Checked = value;
    }

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
    ~NativeUIWin()
		{
      if(components)
        delete components;
      if(notificationIcon)
        delete notificationIcon;
		}

  private: System::Windows::Forms::NotifyIcon^  notificationIcon;
  private: System::ComponentModel::IContainer^  components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
      this->components = (gcnew System::ComponentModel::Container());
      System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
      System::Windows::Forms::Button^  okButton;
      System::Windows::Forms::Button^  cancelButton;
      System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel2;
      System::Windows::Forms::ContextMenuStrip^  notificationMenu;
      System::Windows::Forms::ToolStripMenuItem^  configToolStripMenuItem;
      System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
      System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
      this->mediaCheckBox = (gcnew System::Windows::Forms::CheckBox());
      this->exposeCheckBox = (gcnew System::Windows::Forms::CheckBox());
      this->scrollCheckBox = (gcnew System::Windows::Forms::CheckBox());
      this->notificationIcon = (gcnew System::Windows::Forms::NotifyIcon(this->components));
      this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
      tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
      okButton = (gcnew System::Windows::Forms::Button());
      cancelButton = (gcnew System::Windows::Forms::Button());
      tableLayoutPanel2 = (gcnew System::Windows::Forms::TableLayoutPanel());
      notificationMenu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
      configToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      tableLayoutPanel1->SuspendLayout();
      tableLayoutPanel2->SuspendLayout();
      notificationMenu->SuspendLayout();
      this->SuspendLayout();
      // 
      // tableLayoutPanel1
      // 
      tableLayoutPanel1->ColumnCount = 3;
      tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 100)));
      tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute, 80)));
      tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute, 80)));
      tableLayoutPanel1->Controls->Add(okButton, 1, 1);
      tableLayoutPanel1->Controls->Add(cancelButton, 2, 1);
      tableLayoutPanel1->Controls->Add(tableLayoutPanel2, 0, 0);
      tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
      tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
      tableLayoutPanel1->Name = L"tableLayoutPanel1";
      tableLayoutPanel1->RowCount = 2;
      tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
      tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 31)));
      tableLayoutPanel1->Size = System::Drawing::Size(257, 137);
      tableLayoutPanel1->TabIndex = 1;
      // 
      // okButton
      // 
      okButton->DialogResult = System::Windows::Forms::DialogResult::OK;
      okButton->Location = System::Drawing::Point(100, 109);
      okButton->Name = L"okButton";
      okButton->Size = System::Drawing::Size(74, 23);
      okButton->TabIndex = 0;
      okButton->Text = L"OK";
      okButton->UseVisualStyleBackColor = true;
      okButton->Click += gcnew System::EventHandler(this, &NativeUIWin::okButton_Click);
      // 
      // cancelButton
      // 
      cancelButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
      cancelButton->Location = System::Drawing::Point(180, 109);
      cancelButton->Name = L"cancelButton";
      cancelButton->Size = System::Drawing::Size(74, 23);
      cancelButton->TabIndex = 1;
      cancelButton->Text = L"Cancel";
      cancelButton->UseVisualStyleBackColor = true;
      cancelButton->Click += gcnew System::EventHandler(this, &NativeUIWin::cancelButton_Click);
      // 
      // tableLayoutPanel2
      // 
      tableLayoutPanel2->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
      tableLayoutPanel2->ColumnCount = 1;
      tableLayoutPanel1->SetColumnSpan(tableLayoutPanel2, 3);
      tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 50)));
      tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 50)));
      tableLayoutPanel2->Controls->Add(this->mediaCheckBox, 0, 1);
      tableLayoutPanel2->Controls->Add(this->exposeCheckBox, 1, 0);
      tableLayoutPanel2->Controls->Add(this->scrollCheckBox, 0, 0);
      tableLayoutPanel2->Dock = System::Windows::Forms::DockStyle::Top;
      tableLayoutPanel2->Location = System::Drawing::Point(0, 0);
      tableLayoutPanel2->Margin = System::Windows::Forms::Padding(0);
      tableLayoutPanel2->Name = L"tableLayoutPanel2";
      tableLayoutPanel2->Padding = System::Windows::Forms::Padding(0, 0, 0, 10);
      tableLayoutPanel2->RowCount = 2;
      tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
      tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
      tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
      tableLayoutPanel2->Size = System::Drawing::Size(257, 74);
      tableLayoutPanel2->TabIndex = 2;
      // 
      // mediaCheckBox
      // 
      this->mediaCheckBox->AutoSize = true;
      this->mediaCheckBox->Location = System::Drawing::Point(3, 47);
      this->mediaCheckBox->Name = L"mediaCheckBox";
      this->mediaCheckBox->Size = System::Drawing::Size(132, 14);
      this->mediaCheckBox->TabIndex = 2;
      this->mediaCheckBox->Text = L"Enable Media Controls";
      this->mediaCheckBox->UseVisualStyleBackColor = true;
      this->mediaCheckBox->CheckedChanged += gcnew System::EventHandler(this, &NativeUIWin::mediaCheckBox_CheckedChanged);
      // 
      // exposeCheckBox
      // 
      this->exposeCheckBox->AutoSize = true;
      this->exposeCheckBox->Location = System::Drawing::Point(3, 25);
      this->exposeCheckBox->Name = L"exposeCheckBox";
      this->exposeCheckBox->Size = System::Drawing::Size(148, 16);
      this->exposeCheckBox->TabIndex = 1;
      this->exposeCheckBox->Text = L"Enable Window Selection";
      this->exposeCheckBox->UseVisualStyleBackColor = true;
      this->exposeCheckBox->CheckedChanged += gcnew System::EventHandler(this, &NativeUIWin::exposeCheckBox_CheckedChanged);
      // 
      // scrollCheckBox
      // 
      this->scrollCheckBox->AutoSize = true;
      this->scrollCheckBox->Location = System::Drawing::Point(3, 3);
      this->scrollCheckBox->Name = L"scrollCheckBox";
      this->scrollCheckBox->Size = System::Drawing::Size(102, 16);
      this->scrollCheckBox->TabIndex = 0;
      this->scrollCheckBox->Text = L"Enable Scrolling";
      this->scrollCheckBox->UseVisualStyleBackColor = true;
      this->scrollCheckBox->CheckedChanged += gcnew System::EventHandler(this, &NativeUIWin::scrollCheckBox_CheckedChanged);
      // 
      // notificationMenu
      // 
      notificationMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {
        configToolStripMenuItem,
          helpToolStripMenuItem, this->toolStripSeparator1, exitToolStripMenuItem
      });
      notificationMenu->Name = L"contextMenuStrip1";
      notificationMenu->ShowImageMargin = false;
      notificationMenu->Size = System::Drawing::Size(133, 48);
      // 
      // configToolStripMenuItem
      // 
      configToolStripMenuItem->Name = L"configToolStripMenuItem";
      configToolStripMenuItem->Size = System::Drawing::Size(132, 22);
      configToolStripMenuItem->Text = L"&Configuration...";
      configToolStripMenuItem->Click += gcnew System::EventHandler(this, &NativeUIWin::configToolStripMenuItem_Click);
      // 
      // exitToolStripMenuItem
      // 
      exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
      exitToolStripMenuItem->Size = System::Drawing::Size(132, 22);
      exitToolStripMenuItem->Text = L"&Quit";
      exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &NativeUIWin::exitToolStripMenuItem_Click);
      // 
      // notificationIcon
      // 
      this->notificationIcon->ContextMenuStrip = notificationMenu;
      this->notificationIcon->Text = L"Shortcuts";
      this->notificationIcon->Visible = true;
      // 
      // helpToolStripMenuItem
      // 
      helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
      helpToolStripMenuItem->Size = System::Drawing::Size(132, 22);
      helpToolStripMenuItem->Text = L"&Help";
      helpToolStripMenuItem->Click += gcnew System::EventHandler(this, &NativeUIWin::helpToolStripMenuItem_Click);
      // 
      // toolStripSeparator1
      // 
      this->toolStripSeparator1->Name = L"toolStripSeparator1";
      this->toolStripSeparator1->Size = System::Drawing::Size(129, 6);
      // 
      // NativeUIWin
      // 
      this->AcceptButton = okButton;
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->AutoSize = true;
      this->CancelButton = cancelButton;
      this->ClientSize = System::Drawing::Size(257, 137);
      this->Controls->Add(tableLayoutPanel1);
      this->MaximizeBox = false;
      this->MinimizeBox = false;
      this->Name = L"NativeUIWin";
      this->Text = L"Configuration";
      this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &NativeUIWin::NativeUIWin_FormClosing);
      tableLayoutPanel1->ResumeLayout(false);
      tableLayoutPanel2->ResumeLayout(false);
      tableLayoutPanel2->PerformLayout();
      notificationMenu->ResumeLayout(false);
      this->ResumeLayout(false);

    }
#pragma endregion
  void exitToolStripMenuItem_Click(Object^  sender, System::EventArgs^  e) {
    callbacks.OnQuit();
    Close();
  }
  
  void configToolStripMenuItem_Click(Object^  sender, System::EventArgs^  e) {
    Visible = true;
  }

  void NativeUIWin_FormClosing(Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
    switch(e->CloseReason) {
    case CloseReason::UserClosing:
      callbacks.OnConfigUiHidden(true);
      e->Cancel = true;
      Visible = false;
      break;
    default:
      // If the close reason is anything else, we're getting terminated with good cause.  Instead of
      // treating this as a simple "config dismissed" event, we should instead treat it as an "app
      // quit" event.
      callbacks.OnQuit();
      break;
    }
  }
  void okButton_Click(Object^  sender, System::EventArgs^  e) {
    callbacks.OnConfigUiHidden(false);
    Visible = false;
  }
  void cancelButton_Click(Object^  sender, System::EventArgs^  e) {
    callbacks.OnConfigUiHidden(true);
    Visible = false;
  }
  void helpToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
    callbacks.OnShowHtmlHelp("main");
  }

  System::Void scrollCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
    callbacks.OnSettingChanged("enableScroll", scrollCheckBox->Checked);
  }
  
  System::Void exposeCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
    callbacks.OnSettingChanged("enableWindowSelection", exposeCheckBox->Checked);
  }
  
  System::Void mediaCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
    callbacks.OnSettingChanged("enableMedia", mediaCheckBox->Checked);
  }
};
}
