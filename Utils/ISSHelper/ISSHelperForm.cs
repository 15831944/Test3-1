using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using Microsoft.Win32;

namespace ISSHelper
{
  public delegate void DelegateThreadFinished(string folder, string result);

  public partial class ISSHelperForm : Form
  {
    public string baseFolder;
    public Dictionary<string, string> resultsByFolder = new Dictionary<string, string>();

    public ISSHelperForm()
    {
      InitializeComponent();
    }

    private void SetBaseFolderButton_Click(object sender, EventArgs e)
    {
      RegistryKey registryKey = Registry.CurrentUser.OpenSubKey("Kenwalt\\SysCAD\\ISSHelper");
      if (registryKey != null)
        baseFolderBrowserDialog.SelectedPath = registryKey.GetValue("BaseFolder") as string;

      if (baseFolderBrowserDialog.ShowDialog() == DialogResult.OK)
      {
        baseFolder = baseFolderBrowserDialog.SelectedPath;

        baseFolderTextBox.Text = baseFolder;
        foldersCheckedListBox.Items.Clear();
        resultsTextBox.Text = "";

        registryKey = Registry.CurrentUser.CreateSubKey("Kenwalt\\SysCAD\\ISSHelper");
        registryKey.SetValue("BaseFolder", baseFolderBrowserDialog.SelectedPath);

        foreach (string directory in Directory.GetDirectories(baseFolderBrowserDialog.SelectedPath))
        {
          foldersCheckedListBox.Items.Add(directory);
        }
      }
    }

    private void foldersCheckedListBox_ItemCheck(object sender, ItemCheckEventArgs e)
    {
      string folder = foldersCheckedListBox.Items[e.Index] as string;

      if (resultsByFolder.ContainsKey(folder))
        resultsByFolder.Remove(folder);

      if (e.NewValue == CheckState.Checked)
      { // Add to list.
        foldersCheckedListBox.Enabled = false;
        string result = "";
        ListSubFolders(baseFolder, folder, ref result);
        resultsByFolder.Add(folder, result);
        foldersCheckedListBox.Enabled = true;
      }

      resultsTextBox.Text = "";
      foreach (string result in resultsByFolder.Values)
        resultsTextBox.Text += result;

      Clipboard.Clear();
      if (resultsTextBox.Text.Length > 0)
        Clipboard.SetText(resultsTextBox.Text);
    }

    public void ListSubFolders(string baseFolder, string folder, ref string result)
    {
      foreach (string directory in Directory.GetDirectories(folder))
      {
        string localPath = directory.Remove(0, baseFolder.Length + 1);
        result += "Source: " + localPath + "\\*; DestDir: {app}\\" + localPath + "; Flags: replacesameversion" + Environment.NewLine;
        ListSubFolders(baseFolder, directory, ref result);
        Application.DoEvents();
      }
    }

    private void ISSHelperForm_FormClosing(object sender, FormClosingEventArgs e)
    {
      Environment.Exit(0);
    }
  }
}