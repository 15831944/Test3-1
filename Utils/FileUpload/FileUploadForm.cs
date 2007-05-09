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
    Dictionary<string, int> people = null;

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
      {
        projectsListBox.SelectedItem = registryKey.GetValue("SelectedProject") as string;
        SetNotificationList(registryKey.GetValue("NotificationList") as string);
        //category done later after we have the category list.
        //notification save needs to be added.
        descriptionTextBox.Text = registryKey.GetValue("Description") as string;
        nameTextBox.Text = registryKey.GetValue("Name") as string;
        filenameTextBox.Text = registryKey.GetValue("Filename") as string;
      }

      EnableForm(true);
    }

    private void projectsListBox_SelectedValueChanged(object sender, EventArgs e)
    {
      categoriesListBox.Items.Clear();
      notificationListTreeView.Nodes.Clear();

      EnableForm(false);

      Application.DoEvents();

      // Get upload categories.
      {
        categories = api.GetCategoryList(projects[projectsListBox.SelectedItem as string]);

        foreach (string category in categories.Keys)
          categoriesListBox.Items.Add(category);

        RegistryKey registryKey = Registry.CurrentUser.OpenSubKey("Kenwalt\\SysCAD\\FileUpload");
        if (registryKey != null)
          categoriesListBox.SelectedItem = registryKey.GetValue("SelectedCategory") as string;
      }

      Application.DoEvents();

      // Build person tree.
      {
        people = new Dictionary<string, int>();
        foreach (string company in companies.Keys)
        {
          TreeNode companyNode = notificationListTreeView.Nodes.Add(company);

          Dictionary<string, int> companyPeople = api.GetPersonList(projects[projectsListBox.SelectedItem as string], companies[company]);

          foreach (string person in companyPeople.Keys)
          {
            people.Add(person, companyPeople[person]);
            companyNode.Nodes.Add(person);
          }

          companyNode.Expand();
        }
      }

      EnableForm(true);
    }

    private void EnableForm(bool enabled)
    {
      Enabled = enabled;
      
      foreach (Control control in Controls)
        control.Enabled = true;

      try
      {
        if (!(new FileInfo(filenameTextBox.Text).Exists))
          filenameTextBox.Text = "";
      }
      catch (Exception)
      {
        filenameTextBox.Text = "";
      }

      if ((filenameTextBox.Text.Length == 0)||
          (projectsListBox.SelectedItem == null)||
          (categoriesListBox.SelectedItem == null))
        uploadButton.Enabled = false;
    }

    private void uploadButton_Click(object sender, EventArgs e)
    {
      EnableForm(false);

      RegistryKey registryKey = Registry.CurrentUser.CreateSubKey("Kenwalt\\SysCAD\\FileUpload");
      
      registryKey.SetValue("SelectedProject", projectsListBox.SelectedItem as string);
      registryKey.SetValue("SelectedCategory", categoriesListBox.SelectedItem as string);
      string[] notificationList = GetNotificationList();
      registryKey.SetValue("NotificationList", string.Join(":", notificationList));
      registryKey.SetValue("Description", descriptionTextBox.Text);
      registryKey.SetValue("Name", nameTextBox.Text);
      registryKey.SetValue("Filename", filenameTextBox.Text);

      string fileID = api.UploadFile(filenameTextBox.Text);

      int[] notificationIDList = new int[notificationList.Length];
      for (int i = 0; i < notificationList.Length; i++)
        notificationIDList[i] = people[notificationList[i]];

      api.CreateMessage(projects[projectsListBox.SelectedItem as string],
                        categories[categoriesListBox.SelectedItem as string],
                        notificationIDList,
                        Path.GetFileName(filenameTextBox.Text), 
                        descriptionTextBox.Text, 
                        nameTextBox.Text,
                        fileID);

      EnableForm(true);
    }

    private void SetNotificationList(string notificationString)
    {
      string[] notificationList = notificationString.Split(':');

      ArrayList selectedNodes = new ArrayList();

      foreach (TreeNode companyNode in notificationListTreeView.Nodes)
      {
        foreach (TreeNode node in companyNode.Nodes)
        {
          foreach (string person in notificationList)
          {
            if (node.Text == person)
            selectedNodes.Add(node);
          }
        }
      }
        notificationListTreeView.SelectedNodes = selectedNodes;
    }

    private string[] GetNotificationList()
    {
      string[] notificationList = new string[notificationListTreeView.SelectedNodes.Count];

      for (int i = 0; i < notificationListTreeView.SelectedNodes.Count; i++)
      {
        TreeNode node = notificationListTreeView.SelectedNodes[i] as TreeNode;
        if (node.Nodes.Count == 0) // node is a person, not a company.
        {
          notificationList[i] = node.Text;
        }
      }

      return notificationList;
    }

    private void setFilenameButton_Click(object sender, EventArgs e)
    {
      OpenFileDialog openFileDialog = new OpenFileDialog();

      openFileDialog.CheckFileExists = true;
      openFileDialog.CheckPathExists = true;
      openFileDialog.FileName = filenameTextBox.Text;

      if (openFileDialog.ShowDialog() == DialogResult.OK)
      {
        filenameTextBox.Text = openFileDialog.FileName;

        EnableForm(true);
      }
    }

    private void projectsListBox_SelectedIndexChanged(object sender, EventArgs e)
    {
      EnableForm(true);
    }

    private void categoriesListBox_SelectedIndexChanged(object sender, EventArgs e)
    {
      EnableForm(true);
    }

    private void descriptionTextBox_TextChanged(object sender, EventArgs e)
    {
      EnableForm(true);
    }
  }
}