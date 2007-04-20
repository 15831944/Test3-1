using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Collections;
using System.Security.Cryptography;

namespace Encrypt
{
  public partial class EncryptProjectForm : Form
  {
    public EncryptProjectForm()
    {
      InitializeComponent();
    }

    private void EncryptProjectButton_Click(object sender, EventArgs e)
    {
      OpenFileDialog openFileDialog = new OpenFileDialog();
      openFileDialog.Filter = "Project file|*.spj";
      openFileDialog.FilterIndex = 1;

      ProgressBar.Value = 0;

      string path;
      ArrayList files = new ArrayList();

      if (openFileDialog.ShowDialog() == DialogResult.OK)
      {
        path = Path.GetDirectoryName(openFileDialog.FileName);

        GetFiles(path, files);

        ProgressBar.Minimum = 0;
        ProgressBar.Maximum = files.Count + 1;
        ProgressBar.Value = 1;

        foreach (string file in files)
        {
          Encrypt(file);
          ProgressBar.Value++;
        }

        ProgressBar.Value = 0;
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
            files.Add(file);

        if (Path.GetExtension(file).ToLower() == ".rct")
          if (EncryptRCTFilesCheckBox.Checked)
            files.Add(file);

        //Debugging...
        if (Path.GetExtension(file).ToLower() != ".x")
          if (file.Contains("Scd_Mdls.Mdb"))
            files.Add(file);
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
        for (int i = length - 1; i >= 0; i--)
        {
          encByteArray[i] = (byte)(textByteArray[i] ^ textByteArray[i + 1]);
        }

        fileStream.Close();

        file = file + ".x";
        fileStream = File.Open(file, FileMode.Create);
        if (fileStream.CanWrite)
        {
          fileStream.Write(encByteArray, 0, length);
        }
        else
        {
          MessageBox.Show("Error: Could not write " + file + ".");
        }

        fileStream.Close();
      }
      else
      {
        MessageBox.Show("Error: Could not read " + file + ".");
      }

      fileStream.Close();
    }

  }
}