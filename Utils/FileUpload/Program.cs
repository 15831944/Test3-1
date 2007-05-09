using System;
using System.Collections.Generic;
using System.Windows.Forms;
using BasecampAPI;
using Microsoft.Win32;

namespace FileUpload
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main()
    {
      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);
    
      EnterDetailsForm enterDetailsForm = new BasecampAPI.EnterDetailsForm();

      Dictionary<string, int> projects = null;
      Dictionary<string, int> companies = null;

      do
      {
        RegistryKey registryKey = Registry.CurrentUser.OpenSubKey("Kenwalt\\SysCAD\\FileUpload");
        if (registryKey != null)
        {
          enterDetailsForm.siteTextBox.Text = registryKey.GetValue("Site") as string;
          enterDetailsForm.usernameTextBox.Text = registryKey.GetValue("Username") as string;
          enterDetailsForm.passwordTextBox.Text = registryKey.GetValue("Password") as string;
        }

        enterDetailsForm.ShowDialog();
        if (enterDetailsForm.DialogResult == DialogResult.OK)
        {
          registryKey = Registry.CurrentUser.CreateSubKey("Kenwalt\\SysCAD\\FileUpload");
          registryKey.SetValue("Site", enterDetailsForm.siteTextBox.Text);
          registryKey.SetValue("Username", enterDetailsForm.usernameTextBox.Text);
          registryKey.SetValue("Password", enterDetailsForm.passwordTextBox.Text);

          API api = new API(enterDetailsForm.siteTextBox.Text,
                            enterDetailsForm.usernameTextBox.Text,
                            enterDetailsForm.passwordTextBox.Text);

          projects = api.GetProjectList();
          companies = api.GetCompanyList();

          if ((projects != null) && (companies != null))
            Application.Run(new FileUploadForm(api, projects, companies));
        }
      }
      while ((projects == null) && (enterDetailsForm.DialogResult != DialogResult.Cancel));
    }
  }
}