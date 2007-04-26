using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Collections;

using Microsoft.Win32;

namespace Encrypt
{
  public partial class EncryptProjectForm : Form
  {
    String project0 = null;
    String project1 = null;
    String project2 = null;
    String project3 = null;
    String project4 = null;

    int numberPGM = 0;
    int numberRCT = 0;
    int numberDXF = 0;

    public EncryptProjectForm()
    {
      InitializeComponent();
    }

    private void EncryptProjectButton_Click(object sender, EventArgs e)
    {
      String path = null;
      Boolean backupFailed = false;
      Boolean isProjectFolder = false;

      if (ProjectListBox.SelectedIndex == ProjectListBox.Items.Count-1)
      { // 'Other...' chosen...
        String filename = null; 
        
        OpenFileDialog openFileDialog = new OpenFileDialog();
        openFileDialog.Filter = "Project file|*.spj";
        openFileDialog.FilterIndex = 1;

        if (openFileDialog.ShowDialog() == DialogResult.OK)
          filename = openFileDialog.FileName;

        if (filename != null)
          path = Path.GetDirectoryName(filename);
      }
      else
      {
        path = ProjectListBox.Items[ProjectListBox.SelectedIndex] as String;
      }

      if (path != null)
      {
        FileInfo projectFile = new FileInfo(Path.Combine(path, "Project.spj"));
        isProjectFolder = projectFile.Exists;
        if (!isProjectFolder)
          MessageBox.Show("Not a project folder, cancelling encrypt.");

        if ((isProjectFolder)&&(CreateBackupCheckBox.Checked))
        {
          DirectoryInfo original = new DirectoryInfo(path);
          DirectoryInfo backup = new DirectoryInfo(path + ".backup");

          ProgressToolStripProgressBar.Style = ProgressBarStyle.Marquee;
          try { CopyBackup(original, backup); }
          catch { MessageBox.Show("Backup failed, cancelling encrypt."); backupFailed = true; }
          ProgressToolStripProgressBar.Style = ProgressBarStyle.Continuous;
        }

        if ((isProjectFolder)&&(!backupFailed))
        {
          RegistryKey recentProjectsKey = null;
          try
          {
            recentProjectsKey = Registry.CurrentUser.CreateSubKey("Kenwalt\\SysCAD\\Encrypt\\RecentProjects");
          }
          catch { }

          if (project0 == path)
          { // current project is mose recent already, nothing to do.
          }
          else if (project1 == path)
          {
            project1 = project0;
            project0 = path;
          }
          else if (project2 == path)
          {
            project2 = project1;
            project1 = project0;
            project0 = path;
          }
          else if (project3 == path)
          {
            project3 = project2;
            project2 = project1;
            project1 = project0;
            project0 = path;
          }
          else
          {
            project4 = project3;
            project3 = project2;
            project2 = project1;
            project1 = project0;
            project0 = path;
          }

          if (recentProjectsKey != null)
          {
            try
            {
              if (project0 != null) recentProjectsKey.SetValue("0", project0);
              if (project1 != null) recentProjectsKey.SetValue("1", project1);
              if (project2 != null) recentProjectsKey.SetValue("2", project2);
              if (project3 != null) recentProjectsKey.SetValue("3", project3);
              if (project4 != null) recentProjectsKey.SetValue("4", project4);
            }
            catch { }
          }

          ProjectListBox.Items.Clear();
          if (project0 != null) ProjectListBox.Items.Add(project0);
          if (project1 != null) ProjectListBox.Items.Add(project1);
          if (project2 != null) ProjectListBox.Items.Add(project2);
          if (project3 != null) ProjectListBox.Items.Add(project3);
          if (project4 != null) ProjectListBox.Items.Add(project4);
          ProjectListBox.Items.Add("Other...");
          ProjectListBox.SelectedIndex = 0;

          ProgressToolStripProgressBar.Value = 0;

          ArrayList files = new ArrayList();

          GetFiles(path, files);

          ProgressToolStripProgressBar.Minimum = 0;
          ProgressToolStripProgressBar.Maximum = files.Count + 1;
          ProgressToolStripProgressBar.Value = 1;

          foreach (string file in files)
          {
            Encrypt(file);
            ProgressToolStripProgressBar.Value++;
            toolStripStatusLabel1.Text = "Encrypting " + file;
          }

          ProgressToolStripProgressBar.Value = 0;
          toolStripStatusLabel1.Text = "Completed:  " + numberPGM + " PGM Files, "
                                                      + numberRCT + " RCT Files, "
                                                      + numberDXF + " DXF Files.";
          numberPGM = 0;
          numberRCT = 0;
          numberDXF = 0;
        }
      }
    }

    private void CopyBackup(DirectoryInfo original, DirectoryInfo backup)
    {
      backup.Create();

      foreach (FileInfo file in original.GetFiles())
      {
        file.CopyTo(Path.Combine(backup.ToString(), file.Name), true);
      }

      foreach (DirectoryInfo directory in original.GetDirectories())
      {
        DirectoryInfo subOriginal = new DirectoryInfo(Path.Combine(original.ToString(), directory.Name));
        DirectoryInfo subBackup = new DirectoryInfo(Path.Combine(backup.ToString(), directory.Name));
        CopyBackup(subOriginal, subBackup);
      }
    }

    private void GetFiles(string path, ArrayList files)
    {
      string[] folders = Directory.GetDirectories(path);

      foreach (string folder in folders)
      {
        GetFiles(folder, files);
      }

      foreach (string file in Directory.GetFiles(path))
      {
        if (Path.GetExtension(file).ToLower() == ".pgm")
          if (EncryptPGMFilesCheckBox.Checked)
          {
            files.Add(file);
            numberPGM++;
          }

        if (Path.GetExtension(file).ToLower() == ".rct")
          if (EncryptRCTFilesCheckBox.Checked)
          {
            files.Add(file);
            numberRCT++;
          }

        if (Path.GetExtension(file).ToLower() == ".dxf")
          if (EncryptRCTFilesCheckBox.Checked)
          {
            files.Add(file);
            numberDXF++;
          }

      }
    }

    private void Encrypt(string file)
    {

      FileStream fileStream = null;

      fileStream = File.Open(file, FileMode.Open);
      if (fileStream.CanRead)
      {
        int length = (int)fileStream.Length;
        byte[] textByteArray = new byte[length + 1];
        byte[] encByteArray = new byte[length + 1];

        textByteArray[length] = 71;
        length = fileStream.Read(textByteArray, 0, length);
        fileStream.Close();

        for (int i = length - 1; i >= 0; i--)
          encByteArray[i] = (byte)(textByteArray[i] ^ textByteArray[i + 1]);

        String filex = file + ".x";
        FileStream fileStreamx = File.Open(filex, FileMode.Create);
        if (fileStreamx.CanWrite)
        {
          fileStreamx.Write(encByteArray, 0, length);
          fileStreamx.Close();
          FileInfo fileInfo = new FileInfo(file);
          fileInfo.Delete();
        }
        else
        {
          MessageBox.Show("Error: Could not write " + filex + ".");
        }
      }
      else
      {
        MessageBox.Show("Error: Could not read " + file + ".");
      }
    }

    private void EncryptProjectForm_Load(object sender, EventArgs e)
    {
      RegistryKey recentProjectsKey = Registry.CurrentUser.OpenSubKey("Kenwalt\\SysCAD\\Encrypt\\RecentProjects");
      if (recentProjectsKey != null)
      {
        project0 = recentProjectsKey.GetValue("0") as String;
        project1 = recentProjectsKey.GetValue("1") as String;
        project2 = recentProjectsKey.GetValue("2") as String;
        project3 = recentProjectsKey.GetValue("3") as String;
        project4 = recentProjectsKey.GetValue("4") as String;
      }

      ProjectListBox.Items.Clear();
      if (project0 != null) ProjectListBox.Items.Add(project0);
      if (project1 != null) ProjectListBox.Items.Add(project1);
      if (project2 != null) ProjectListBox.Items.Add(project2);
      if (project3 != null) ProjectListBox.Items.Add(project3);
      if (project4 != null) ProjectListBox.Items.Add(project4);
      ProjectListBox.Items.Add("Other...");
      ProjectListBox.SelectedIndex = 0;
    }

    private void ProjectListBox_DoubleClick(object sender, EventArgs e)
    {
      EncryptProjectButton_Click(sender, e);
    }
  }
}