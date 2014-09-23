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

  public:



  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
  private: System::Windows::Forms::CheckBox^  mediaCheckBox;
  private: System::Windows::Forms::CheckBox^  exposeCheckBox;
  private: System::Windows::Forms::CheckBox^  scrollCheckBox;
  private: System::Windows::Forms::GroupBox^  box_featureSelection;


  private: System::Windows::Forms::TrackBar^  scrollSensitivityBar;

  private: System::Windows::Forms::Label^  label_scrollSensitivity;

  private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
  private: System::Windows::Forms::Button^  button_ok;

  private: System::Windows::Forms::Label^  label_version;




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
      else if (var == "showHelpOnStart") {
        if (value) {
          s_nativeUI->callbacks.OnShowHtmlHelp("main");
          s_nativeUI->callbacks.OnSettingChanged("showHelpOnStart", false);
        }
      }
      
    }

    static void ConfigChanged(const std::string& var, double value) {
      if (var == "scrollSensitivity") {
        s_nativeUI->trackBar1->Value = (int)value;
      }
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
      System::Windows::Forms::ContextMenuStrip^  notificationMenu;
      System::Windows::Forms::ToolStripMenuItem^  configToolStripMenuItem;
      System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
      System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
      this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
      this->notificationIcon = (gcnew System::Windows::Forms::NotifyIcon(this->components));
      this->mediaCheckBox = (gcnew System::Windows::Forms::CheckBox());
      this->exposeCheckBox = (gcnew System::Windows::Forms::CheckBox());
      this->scrollCheckBox = (gcnew System::Windows::Forms::CheckBox());
      this->box_featureSelection = (gcnew System::Windows::Forms::GroupBox());
      this->scrollSensitivityBar = (gcnew System::Windows::Forms::TrackBar());
      this->label_scrollSensitivity = (gcnew System::Windows::Forms::Label());
      this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
      this->button_ok = (gcnew System::Windows::Forms::Button());
      this->label_version = (gcnew System::Windows::Forms::Label());
      notificationMenu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
      configToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      notificationMenu->SuspendLayout();
      this->box_featureSelection->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->scrollSensitivityBar))->BeginInit();
      this->tableLayoutPanel1->SuspendLayout();
      this->SuspendLayout();
      // 
      // notificationMenu
      // 
      notificationMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {
        configToolStripMenuItem,
          helpToolStripMenuItem, this->toolStripSeparator1, exitToolStripMenuItem
      });
      notificationMenu->Name = L"contextMenuStrip1";
      notificationMenu->ShowImageMargin = false;
      notificationMenu->Size = System::Drawing::Size(228, 118);
      // 
      // configToolStripMenuItem
      // 
      configToolStripMenuItem->Name = L"configToolStripMenuItem";
      configToolStripMenuItem->Size = System::Drawing::Size(227, 36);
      configToolStripMenuItem->Text = L"&Configuration...";
      configToolStripMenuItem->Click += gcnew System::EventHandler(this, &NativeUIWin::configToolStripMenuItem_Click);
      // 
      // helpToolStripMenuItem
      // 
      helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
      helpToolStripMenuItem->Size = System::Drawing::Size(227, 36);
      helpToolStripMenuItem->Text = L"&Help";
      helpToolStripMenuItem->Click += gcnew System::EventHandler(this, &NativeUIWin::helpToolStripMenuItem_Click);
      // 
      // toolStripSeparator1
      // 
      this->toolStripSeparator1->Name = L"toolStripSeparator1";
      this->toolStripSeparator1->Size = System::Drawing::Size(224, 6);
      // 
      // exitToolStripMenuItem
      // 
      exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
      exitToolStripMenuItem->Size = System::Drawing::Size(227, 36);
      exitToolStripMenuItem->Text = L"&Quit";
      exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &NativeUIWin::exitToolStripMenuItem_Click);
      // 
      // notificationIcon
      // 
      this->notificationIcon->ContextMenuStrip = notificationMenu;
      this->notificationIcon->Text = L"Shortcuts";
      this->notificationIcon->Visible = true;
      // 
      // mediaCheckBox
      // 
      this->mediaCheckBox->AutoSize = true;
      this->mediaCheckBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.5F));
      this->mediaCheckBox->Location = System::Drawing::Point(22, 149);
      this->mediaCheckBox->Margin = System::Windows::Forms::Padding(6);
      this->mediaCheckBox->Name = L"mediaCheckBox";
      this->mediaCheckBox->Size = System::Drawing::Size(242, 29);
      this->mediaCheckBox->TabIndex = 5;
      this->mediaCheckBox->Text = L"Enable Media Controls";
      this->mediaCheckBox->UseVisualStyleBackColor = true;
      // 
      // exposeCheckBox
      // 
      this->exposeCheckBox->AutoSize = true;
      this->exposeCheckBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.5F));
      this->exposeCheckBox->Location = System::Drawing::Point(22, 98);
      this->exposeCheckBox->Margin = System::Windows::Forms::Padding(6);
      this->exposeCheckBox->Name = L"exposeCheckBox";
      this->exposeCheckBox->Size = System::Drawing::Size(267, 29);
      this->exposeCheckBox->TabIndex = 4;
      this->exposeCheckBox->Text = L"Enable Window Selection";
      this->exposeCheckBox->UseVisualStyleBackColor = true;
      this->exposeCheckBox->CheckedChanged += gcnew System::EventHandler(this, &NativeUIWin::exposeCheckBox_CheckedChanged);
      // 
      // scrollCheckBox
      // 
      this->scrollCheckBox->AutoSize = true;
      this->scrollCheckBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.5F));
      this->scrollCheckBox->Location = System::Drawing::Point(22, 49);
      this->scrollCheckBox->Margin = System::Windows::Forms::Padding(6);
      this->scrollCheckBox->Name = L"scrollCheckBox";
      this->scrollCheckBox->Size = System::Drawing::Size(185, 29);
      this->scrollCheckBox->TabIndex = 3;
      this->scrollCheckBox->Text = L"Enable Scrolling";
      this->scrollCheckBox->UseVisualStyleBackColor = true;
      // 
      // box_featureSelection
      // 
      this->box_featureSelection->Controls->Add(this->exposeCheckBox);
      this->box_featureSelection->Controls->Add(this->scrollCheckBox);
      this->box_featureSelection->Controls->Add(this->mediaCheckBox);
      this->box_featureSelection->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9));
      this->box_featureSelection->Location = System::Drawing::Point(28, 25);
      this->box_featureSelection->Name = L"box_featureSelection";
      this->box_featureSelection->Size = System::Drawing::Size(338, 206);
      this->box_featureSelection->TabIndex = 6;
      this->box_featureSelection->TabStop = false;
      this->box_featureSelection->Text = L"Feature Selection";
      // 
      // scrollSensitivityBar
      // 
      this->scrollSensitivityBar->Location = System::Drawing::Point(3, 3);
      this->scrollSensitivityBar->Maximum = 20;
      this->scrollSensitivityBar->Minimum = 5;
      this->scrollSensitivityBar->Name = L"scrollSensitivityBar";
      this->scrollSensitivityBar->Size = System::Drawing::Size(398, 53);
      this->scrollSensitivityBar->TabIndex = 6;
      this->scrollSensitivityBar->Value = 12;
      this->scrollSensitivityBar->MouseCaptureChanged += gcnew System::EventHandler(this, &NativeUIWin::trackBar1_MouseCaptureChanged);
      // 
      // label_scrollSensitivity
      // 
      this->label_scrollSensitivity->Anchor = System::Windows::Forms::AnchorStyles::Top;
      this->label_scrollSensitivity->AutoSize = true;
      this->label_scrollSensitivity->Location = System::Drawing::Point(117, 59);
      this->label_scrollSensitivity->Name = L"label_scrollSensitivity";
      this->label_scrollSensitivity->Size = System::Drawing::Size(170, 25);
      this->label_scrollSensitivity->TabIndex = 7;
      this->label_scrollSensitivity->Text = L"Scroll Sensitivity";
      // 
      // tableLayoutPanel1
      // 
      this->tableLayoutPanel1->ColumnCount = 1;
      this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
        50)));
      this->tableLayoutPanel1->Controls->Add(this->scrollSensitivityBar, 0, 0);
      this->tableLayoutPanel1->Controls->Add(this->label_scrollSensitivity, 0, 1);
      this->tableLayoutPanel1->Location = System::Drawing::Point(395, 40);
      this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
      this->tableLayoutPanel1->RowCount = 2;
      this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
      this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 36)));
      this->tableLayoutPanel1->Size = System::Drawing::Size(404, 95);
      this->tableLayoutPanel1->TabIndex = 8;
      // 
      // button_ok
      // 
      this->button_ok->Location = System::Drawing::Point(640, 172);
      this->button_ok->Name = L"button_ok";
      this->button_ok->Size = System::Drawing::Size(157, 61);
      this->button_ok->TabIndex = 9;
      this->button_ok->Text = L"Ok";
      this->button_ok->UseVisualStyleBackColor = true;
      // 
      // label_version
      // 
      this->label_version->AutoSize = true;
      this->label_version->Location = System::Drawing::Point(393, 190);
      this->label_version->Name = L"label_version";
      this->label_version->Size = System::Drawing::Size(221, 25);
      this->label_version->TabIndex = 10;
      this->label_version->Text = L"Shortcuts version: 1.0";
      // 
      // NativeUIWin
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(12, 25);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->AutoSize = true;
      this->ClientSize = System::Drawing::Size(828, 269);
      this->Controls->Add(this->label_version);
      this->Controls->Add(this->button_ok);
      this->Controls->Add(this->tableLayoutPanel1);
      this->Controls->Add(this->box_featureSelection);
      this->Margin = System::Windows::Forms::Padding(6);
      this->MaximizeBox = false;
      this->MinimizeBox = false;
      this->Name = L"NativeUIWin";
      this->Text = L"Shortcuts Configuration";
      this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &NativeUIWin::NativeUIWin_FormClosing);
      notificationMenu->ResumeLayout(false);
      this->box_featureSelection->ResumeLayout(false);
      this->box_featureSelection->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->scrollSensitivityBar))->EndInit();
      this->tableLayoutPanel1->ResumeLayout(false);
      this->tableLayoutPanel1->PerformLayout();
      this->ResumeLayout(false);
      this->PerformLayout();

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
  System::Void trackBar1_MouseCaptureChanged(System::Object^  sender, System::EventArgs^  e) {
    callbacks.OnSettingChanged("scrollSensitivity", (double)trackBar1->Value);
  }
};
}
