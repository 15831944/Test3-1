using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using System.IO;
using System.IO.Compression;
using System.Security.Cryptography;

namespace Enc
{
  public partial class EncForm : Form
  {
    byte[] buffer = new byte[16*1024]; // 64KB

    public EncForm()
    {
      InitializeComponent();
    }

    private void buttonEncrypt_Click(object sender, EventArgs e)
    {
      OpenFileDialog openFileDialog = new OpenFileDialog();
      openFileDialog.CheckFileExists = true;
      openFileDialog.CheckPathExists = true;

      if (openFileDialog.ShowDialog() == DialogResult.OK)
      {
        FileStream origFileStream = new FileStream(openFileDialog.FileName, FileMode.Open);
        FileStream encoutFileStream = new FileStream(openFileDialog.FileName+".enc", FileMode.OpenOrCreate);

        SHA512Managed sha512 = new SHA512Managed();
        System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
        byte[] hash = sha512.ComputeHash(encoding.GetBytes(textBoxPassword.Text));
        encoutFileStream.Write(hash, 0, hash.Length);

        GZipStream gzipoutStream = new GZipStream(encoutFileStream, CompressionMode.Compress);
        
        int count = origFileStream.Read(buffer, 0, buffer.Length);
        while (count > 0)
        {
          gzipoutStream.Write(buffer, 0, count);
          count = origFileStream.Read(buffer, 0, buffer.Length);
        }
        
        origFileStream.Close();
        gzipoutStream.Close();
        encoutFileStream.Close();
      }
    }

    private void buttonDecrypt_Click(object sender, EventArgs e)
    {
      OpenFileDialog openFileDialog = new OpenFileDialog();
      openFileDialog.CheckFileExists = true;
      openFileDialog.CheckPathExists = true;
      openFileDialog.Filter = "Encrypted Files|*.enc";

      if( openFileDialog.ShowDialog() == DialogResult.OK )
      {
        FileStream encinFileStream = new FileStream(openFileDialog.FileName, FileMode.Open);

        SHA512Managed sha512 = new SHA512Managed();
        System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
        byte[] localHash = sha512.ComputeHash(encoding.GetBytes(textBoxPassword.Text));
        byte[] fileHash = new byte[localHash.Length];
        encinFileStream.Read(fileHash, 0, fileHash.Length);

        for (int i = 0; i < fileHash.Length; i++)
          if (fileHash[i] != localHash[i])
          {
            MessageBox.Show("Incorrect Password.");
            return;
          }
        
        GZipStream gzipinStream = new GZipStream(encinFileStream, CompressionMode.Decompress);

        FileStream unencFileStream = new FileStream(openFileDialog.FileName.Remove(openFileDialog.FileName.Length - 4)+".unenc", FileMode.OpenOrCreate);

        int count = gzipinStream.Read(buffer, 0, buffer.Length);
        while (count > 0)
        {
          unencFileStream.Write(buffer, 0, count);
          count = gzipinStream.Read(buffer, 0, buffer.Length);
        }

        gzipinStream.Close();
        encinFileStream.Close();
        unencFileStream.Close();
      }
    }
  }
}