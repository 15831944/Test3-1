using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Threading;
using Microsoft.Win32;

namespace ISSHelper
{
  public partial class iSSHelperForm : Form
  {
    public iSSHelperForm()
    {
      InitializeComponent();
    }

    private void SetBaseFolderButton_Click(object sender, EventArgs e)
    {
      RegistryKey registryKey = Registry.CurrentUser.OpenSubKey("Kenwalt\\SysCAD\\ISSHelper");
      baseFolderBrowserDialog.SelectedPath = registryKey.GetValue("BaseFolder") as string;

      if (baseFolderBrowserDialog.ShowDialog() == DialogResult.OK)
      {
        registryKey = Registry.CurrentUser.CreateSubKey("Kenwalt\\SysCAD\\ISSHelper");
        registryKey.SetValue("BaseFolder", baseFolderBrowserDialog.SelectedPath);

        baseFolderTextBox.Text = baseFolderBrowserDialog.SelectedPath;
        resultsTextBox.Text = "";

        ListSubFolders(baseFolderBrowserDialog.SelectedPath);

        Clipboard.SetText(resultsTextBox.Text);
      }
    }

    private void ListSubFolders(string folder)
    {
      foreach (string directory in Directory.GetDirectories(folder))
      {
        string localPath = directory.Remove(0, baseFolderTextBox.Text.Length + 1);
        resultsTextBox.Text += "Source: " + localPath + "\\*; DestDir: {app}\\" + localPath + "; Flags: replacesameversion" + Environment.NewLine;
        ListSubFolders(directory);
      }
    }
  }
}