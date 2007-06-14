
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using SysCAD.Protocol;

//using System.Security.Permissions;

namespace SysCAD.Editor
{

  internal partial class OpenProjectForm : Form
  {
    private ClientProtocol clientProtocol = new ClientProtocol();

    private Config config = new Config();

    public OpenProjectForm()
    {
      InitializeComponent();

      if (config.TestUrl(new System.Uri(repositoryURLTextBox.Text + "Global")))
      {
        config.GetProjectList();
        projectListBox.Items.Clear();

        foreach (String projectString in config.ProjectList)
        {
          projectListBox.Items.Add(projectString);
        }

        projectListBox.Enabled = true;
        projectListBox.SelectedIndex = 0;
      }
    }

    private void listProjectsButton_Click(object sender, EventArgs e)
    {

      if (config.TestUrl(new System.Uri(repositoryURLTextBox.Text + "Global")))
      {
        config.GetProjectList();
        projectListBox.Items.Clear();

        foreach (String projectString in config.ProjectList)
        {
          projectListBox.Items.Add(projectString);
        }

        projectListBox.Enabled = true;
      }

      else
      {
        projectListBox.Items.Clear();
      }
      errorProvider1.SetError(projectListBox, config.connectionError);
    }

    private void projectListBox_DoubleClick(object sender, EventArgs e)
    {
      openButton.PerformClick();
    }

    private void projectListBox_KeyPress(object sender, KeyPressEventArgs e)
    {

      if (e.KeyChar == '\r')
      {
        openButton.PerformClick();
      }
      e.Handled = true;
    }

    private void projectListBox_SelectedValueChanged(object sender, EventArgs e)
    {

      if (projectListBox.SelectedItem != null)
      {

        if (clientProtocol.TestUrl(new System.Uri(repositoryURLTextBox.Text + "Client/" + projectListBox.SelectedItem.ToString())))
        {
          openButton.Enabled = true;
        }

        errorProvider1.SetError(projectListBox, clientProtocol.connectionError);
      }
    }

    private void repositoryURLTextBox_KeyPress(object sender, KeyPressEventArgs e)
    {

      if (e.KeyChar == '\r')
      {
        listProjectsButton.PerformClick();
        e.Handled = true;
      }
    }

    public ClientProtocol ClientProtocol
    {
      get { return clientProtocol; }
    }

    public Config Config
    {
      get { return config; }
    }
  }
}
