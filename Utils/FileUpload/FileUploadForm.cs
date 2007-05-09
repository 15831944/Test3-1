using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using BasecampAPI;
using Microsoft.Win32;
using System.Collections;
using System.IO;

namespace FileUpload
{
  public partial class FileUploadForm : Form
  {
    API api = null;

    Dictionary<string, int> projects = null;
    Dictionary<string, int> categories = null;
    Dictionary<string, int> companies = null;
    Dictionary<string, Dictionary<string, int>> people = null;

    public FileUploadForm(API api, Dictionary<string, int> projects, Dictionary<string, int> companies)
    {
      this.api = api;
      this.projects = projects;
      this.companies = companies;

      InitializeComponent();

      foreach (string project in projects.Keys)
        projectsListBox.Items.Add(project);

      RegistryKey registryKey = Registry.CurrentUser.OpenSubKey("Kenwalt\\SysCAD\\FileUpload");
      if (registryKey != null)
        projectsListBox.SelectedItem = registryKey.GetValue("SelectedProject") as string;
    }

    private void projectsListBox_SelectedValueChanged(object sender, EventArgs e)
    {
      categoriesListBox.Items.Clear();
      notificationListTreeView.Nodes.Clear();

      foreach (Control control in Controls)
        control.Enabled = false;
      Enabled = false;

      Application.DoEvents();

      RegistryKey registryKey = Registry.CurrentUser.CreateSubKey("Kenwalt\\SysCAD\\FileUpload");
      registryKey.SetValue("SelectedProject", projectsListBox.SelectedItem as string);

      // Get upload categories.
      {
        categories = api.GetAttachmentCategoryList(projects[projectsListBox.SelectedItem as string]);

        foreach (string category in categories.Keys)
          categoriesListBox.Items.Add(category);

        registryKey = Registry.CurrentUser.OpenSubKey("Kenwalt\\SysCAD\\FileUpload");
        if (registryKey != null)
          categoriesListBox.SelectedItem = registryKey.GetValue("SelectedAttachmentCategory") as string;
      }

      Application.DoEvents();

      // Build person tree.
      {
        people = new Dictionary<string, Dictionary<string, int>>();
        foreach (string company in companies.Keys)
        {
          TreeNode companyNode = notificationListTreeView.Nodes.Add(company);

          people.Add(company, api.GetPersonList(projects[projectsListBox.SelectedItem as string], companies[company]));

          foreach (string person in people[company].Keys)
          {
            companyNode.Nodes.Add(person);
          }
        }
      }

      Enabled = true;
      foreach (Control control in Controls)
        control.Enabled = true;
    }

    private void categoriesListBox_SelectedValueChanged(object sender, EventArgs e)
    {
      RegistryKey registryKey = Registry.CurrentUser.CreateSubKey("Kenwalt\\SysCAD\\FileUpload");
      registryKey.SetValue("SelectedAttachmentCategory", categoriesListBox.SelectedItem as string);
    }

    private void uploadButton_Click(object sender, EventArgs e)
    {
      foreach (Control control in Controls)
        control.Enabled = false;
      Enabled = false;

      string fileName = "c:\\svn.h";

      string fileID = api.UploadFile(fileName);

      api.CreateMessage(projects[projectsListBox.SelectedItem as string],
                        categories[categoriesListBox.SelectedItem as string],
                        Path.GetFileName(fileName), descriptionTextBox.Text, fileID);

      Enabled = true;
      foreach (Control control in Controls)
        control.Enabled = true;
    }
  }
}