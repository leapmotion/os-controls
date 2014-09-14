#pragma once

#if _MANAGED

namespace oscontrols {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for NativeUI
	/// </summary>
	public ref class NativeUI : public System::Windows::Forms::Form
	{
	public:
		NativeUI(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~NativeUI()
		{
			if (components)
			{
				delete components;
			}
		}
  private: System::Windows::Forms::NotifyIcon^  notifyIcon1;
  protected:
  private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip1;
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
      System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(NativeUI::typeid));
      this->notifyIcon1 = (gcnew System::Windows::Forms::NotifyIcon(this->components));
      this->contextMenuStrip1 = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
      this->configToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->contextMenuStrip1->SuspendLayout();
      this->SuspendLayout();
      // 
      // notifyIcon1
      // 
      this->notifyIcon1->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"notifyIcon1.Icon")));
      this->notifyIcon1->Text = L"Leap Hand Control";
      this->notifyIcon1->Visible = true;
      // 
      // contextMenuStrip1
      // 
      this->contextMenuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
        this->configToolStripMenuItem,
          this->exitToolStripMenuItem
      });
      this->contextMenuStrip1->Name = L"contextMenuStrip1";
      this->contextMenuStrip1->ShowImageMargin = false;
      this->contextMenuStrip1->Size = System::Drawing::Size(128, 70);
      // 
      // configToolStripMenuItem
      // 
      this->configToolStripMenuItem->Name = L"configToolStripMenuItem";
      this->configToolStripMenuItem->Size = System::Drawing::Size(127, 22);
      this->configToolStripMenuItem->Text = L"Config..";
      // 
      // exitToolStripMenuItem
      // 
      this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
      this->exitToolStripMenuItem->Size = System::Drawing::Size(127, 22);
      this->exitToolStripMenuItem->Text = L"Quit";
      // 
      // NativeUI
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(284, 262);
      this->Name = L"NativeUI";
      this->Text = L"NativeUI";
      this->contextMenuStrip1->ResumeLayout(false);
      this->ResumeLayout(false);

    }
#pragma endregion
	};
}
#endif

void ShowUI(void);