#pragma once
#include "NativeUI.h"

struct NativeUI;

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
    NativeUIWin(NativeUI& callbacks):
      callbacks(callbacks)
		{
			InitializeComponent();
			
      ResourceManager^ rm = gcnew ResourceManager("oscontrols.Resource", Assembly::GetExecutingAssembly());
      System::Drawing::Bitmap^ appBmp = (System::Drawing::Bitmap^) rm->GetObject("icon_512x512");
      this->Icon = System::Drawing::Icon::FromHandle(appBmp->GetHicon());

      System::Drawing::Bitmap^ trayBmp = (System::Drawing::Bitmap^) rm->GetObject("TrayIcon_20x18");
      this->notificationIcon->Icon = System::Drawing::Icon::FromHandle(trayBmp->GetHicon());
		}

    static size_t s_nativeUIInitCount = 0;
    NativeUI& callbacks;

  public:
    static NativeUIWin^ s_nativeUI;

    static void AddTrayIcon(NativeUI& callbacks) {
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
      if(components)
        delete components;
      if(notificationIcon)
        delete notificationIcon;
		}

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
      System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
      System::Windows::Forms::Button^  okButton;
      System::Windows::Forms::Button^  cancelButton;
      System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel2;
      this->notificationIcon = (gcnew System::Windows::Forms::NotifyIcon(this->components));
      this->notificationMenu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
      this->configToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
      okButton = (gcnew System::Windows::Forms::Button());
      cancelButton = (gcnew System::Windows::Forms::Button());
      tableLayoutPanel2 = (gcnew System::Windows::Forms::TableLayoutPanel());
      tableLayoutPanel1->SuspendLayout();
      this->notificationMenu->SuspendLayout();
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
      tableLayoutPanel1->Size = System::Drawing::Size(418, 409);
      tableLayoutPanel1->TabIndex = 1;
      // 
      // okButton
      // 
      okButton->DialogResult = System::Windows::Forms::DialogResult::OK;
      okButton->Location = System::Drawing::Point(261, 381);
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
      cancelButton->Location = System::Drawing::Point(341, 381);
      cancelButton->Name = L"cancelButton";
      cancelButton->Size = System::Drawing::Size(74, 23);
      cancelButton->TabIndex = 1;
      cancelButton->Text = L"Cancel";
      cancelButton->UseVisualStyleBackColor = true;
      cancelButton->Click += gcnew System::EventHandler(this, &NativeUIWin::cancelButton_Click);
      // 
      // tableLayoutPanel2
      // 
      tableLayoutPanel2->ColumnCount = 2;
      tableLayoutPanel1->SetColumnSpan(tableLayoutPanel2, 3);
      tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 50)));
      tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 50)));
      tableLayoutPanel2->Dock = System::Windows::Forms::DockStyle::Fill;
      tableLayoutPanel2->Location = System::Drawing::Point(0, 0);
      tableLayoutPanel2->Margin = System::Windows::Forms::Padding(0);
      tableLayoutPanel2->Name = L"tableLayoutPanel2";
      tableLayoutPanel2->RowCount = 2;
      tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
      tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
      tableLayoutPanel2->Size = System::Drawing::Size(418, 378);
      tableLayoutPanel2->TabIndex = 2;
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
      this->AcceptButton = okButton;
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->CancelButton = cancelButton;
      this->ClientSize = System::Drawing::Size(418, 409);
      this->Controls->Add(tableLayoutPanel1);
      this->MaximizeBox = false;
      this->MinimizeBox = false;
      this->Name = L"NativeUIWin";
      this->Text = L"Configuration";
      this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &NativeUIWin::NativeUIWin_FormClosing);
      tableLayoutPanel1->ResumeLayout(false);
      this->notificationMenu->ResumeLayout(false);
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
};
}
