using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Security.Cryptography;

namespace RestrictPermissions
{
  public partial class Form1 : Form
  {
    string filename = null;

    string restrictions = "";

    byte[] metricHash;

    public Form1()
    {
      InitializeComponent();
    }

    private void button1_Click(object sender, EventArgs e)
    {
      filename = null;

      setButton.Enabled = false;
      passwordTextBox.Enabled = false;
      ReadOnlyCheckBox.Enabled = false;

      encryptButton.Enabled = false;
      decryptButton.Enabled = false;

      checkBox1.Enabled = false;
      checkBox2.Enabled = false;
      checkBox3.Enabled = false;
      checkBox4.Enabled = false;
      checkBox5.Enabled = false;
      checkBox6.Enabled = false;
      checkBox7.Enabled = false;

      OpenFileDialog openDialog = new OpenFileDialog();
      openDialog.Title = "Open Project Configuration File";
      openDialog.Filter = "Project Configuration Files|*.spj";
      openDialog.AddExtension = true;
      openDialog.CheckFileExists = true;
      openDialog.CheckPathExists = true;
      if (openDialog.ShowDialog() == DialogResult.OK)
      {
        FileStream fileStream = new FileStream(openDialog.FileName, FileMode.Open);
        StreamReader streamReader = new StreamReader(fileStream);
        string fileString = streamReader.ReadToEnd();
        streamReader.Close();
        fileStream.Close();

        metricHash = GetMetricHash(fileString);

        byte[] passwordHash = null;
        restrictions = "";
        byte[] restrictionCheck = null;

        string[] splitter = { "\r\n" };
        string[] fileStrings = fileString.Split(splitter, StringSplitOptions.RemoveEmptyEntries);

        foreach (string lineString in fileStrings)
        {
          if (lineString.Contains("PasswordHash="))
            passwordHash = GetPasswordHash(lineString);
          else if (lineString.Contains("Restrictions="))
            restrictions = GetRestrictions(lineString);
          else if (lineString.Contains("RestrictionsCheck="))
            restrictionCheck = GetRestrictionsCheck(lineString);
        }

        if (!ValidRestrictionCheck(passwordHash, restrictions, metricHash, restrictionCheck))
        {
          MessageBox.Show("Invalid Restriction Information");
          return;
        }

        string enteredPassword = "";
        if (!PasswordCheck(enteredPassword, passwordHash))
        {
          passwordDlg passForm = new passwordDlg();
          if (passForm.ShowDialog(this) == DialogResult.OK)
            enteredPassword = passForm.getPass();
        }

        if (PasswordCheck(enteredPassword, passwordHash))
        {
          passwordTextBox.Text = enteredPassword;

          filename = openDialog.FileName;

          setButton.Enabled = true;
          passwordTextBox.Enabled = true;
          ReadOnlyCheckBox.Enabled = true;

          encryptButton.Enabled = true;
          decryptButton.Enabled = true;

          checkBox1.Enabled = true;
          checkBox2.Enabled = true;
          checkBox3.Enabled = true;
          checkBox4.Enabled = true;
          checkBox5.Enabled = true;
          checkBox6.Enabled = true;
          checkBox7.Enabled = true;

          if (restrictions.Contains(ReadOnlyCheckBox.Text))
            ReadOnlyCheckBox.Checked = true;
          else
            ReadOnlyCheckBox.Checked = false;

          if (restrictions.Contains(checkBox1.Text))
            checkBox1.Checked = true;
          else
            checkBox1.Checked = false;

          if (restrictions.Contains(checkBox2.Text))
            checkBox2.Checked = true;
          else
            checkBox2.Checked = false;

          if (restrictions.Contains(checkBox3.Text))
            checkBox3.Checked = true;
          else
            checkBox3.Checked = false;

          if (restrictions.Contains(checkBox4.Text))
            checkBox4.Checked = true;
          else
            checkBox4.Checked = false;

          if (restrictions.Contains(checkBox5.Text))
            checkBox5.Checked = true;
          else
            checkBox5.Checked = false;

          if (restrictions.Contains(checkBox6.Text))
            checkBox6.Checked = true;
          else
            checkBox6.Checked = false;

          if (restrictions.Contains(checkBox7.Text))
            checkBox7.Checked = true;
          else
            checkBox7.Checked = false;

          return;
        }
        else
        {
          MessageBox.Show("Invalid Password");
          return;
        }
      }
    }

    private bool PasswordCheck(string enteredPassword, byte[] passwordHash)
    {
      byte[] enteredPasswordHash = null;
      if (enteredPassword != null)
        enteredPasswordHash = ComputeHash(enteredPassword);

      if ((passwordHash != null) && (enteredPasswordHash != null) && (passwordHash.Length == enteredPasswordHash.Length))
      {
        for (int i = 0; i < passwordHash.Length; i++)
        {
          if (enteredPasswordHash[i] != passwordHash[i])
            return false;
        }
        return true;
      }

      return false;
    }

    private string HashToHex(byte[] hash)
    {
      string hex = BitConverter.ToString(hash);
      hex = hex.Replace("-", "");
      if (hex.Length == 128)
        return hex;
      else
        return null;
    }

    private byte[] HexToHash(string hexString)
    {
      if (hexString.Length == 128)
      {
        byte[] bytes = new byte[hexString.Length / 2];
        for (int i = 0; i < hexString.Length; i += 2)
        {
          try
          {
            bytes[i / 2] = Convert.ToByte(hexString.Substring(i, 2), 16);
          }
          catch
          {
            return null;
          }
        }
        return bytes;
      }
      else
        return null;
    }

    private bool ValidRestrictionCheck(byte[] passwordHash, string restrictions, byte[] metricHash, byte[] restrictionCheck)
    {
      if ((passwordHash != null) && (restrictionCheck != null) && (passwordHash.Length == restrictionCheck.Length))
      {
        byte[] calculatedRestrictionCheck = CalculateRestrictionCheck(passwordHash, restrictions, metricHash);
        for (int i = 0; i < passwordHash.Length; i++)
        {
          if (calculatedRestrictionCheck[i] != restrictionCheck[i])
            return false;
        }
        return true;
      }
      return false;
    }

    private byte[] CalculateRestrictionCheck(byte[] passwordHash, string restrictions, byte[] metricHash)
    {
      return ComputeHash(HashToHex(passwordHash) + restrictions + HashToHex(metricHash));
    }

    private byte[] ComputeHash(string str)
    {
      SHA512 sha512 = SHA512.Create();
      return sha512.ComputeHash((new ASCIIEncoding()).GetBytes(str));
    }

    private byte[] GetMetricHash(string fileString)
    {
      string metric = "Metric!";
      // Calculate metric first...
      return ComputeHash(metric);
    }

    private byte[] GetRestrictionsCheck(string lineString)
    {
      lineString = lineString.Replace("RestrictionsCheck=", "");
      return HexToHash(lineString);
    }

    private string GetRestrictions(string lineString)
    {
      lineString = lineString.Replace("Restrictions=", "");
      return lineString;
    }

    private byte[] GetPasswordHash(string lineString)
    {
      lineString = lineString.Replace("PasswordHash=", "");
      return HexToHash(lineString);
    }

    private void setButton_Click(object sender, EventArgs e)
    {
      if (filename != null)
      {
        FileStream inFileStream = new FileStream(filename, FileMode.Open);
        StreamReader streamReader = new StreamReader(inFileStream);
        string fileString = streamReader.ReadToEnd();
        streamReader.Close();
        inFileStream.Close();

        //set on open --- byte[] metricHash = GetMetricHash(fileString);
        string metricHashHex = HashToHex(metricHash);

        byte[] passwordHash = ComputeHash(passwordTextBox.Text);
        string passwordHashHex = HashToHex(passwordHash);
        byte[] restrictionCheck = CalculateRestrictionCheck(passwordHash, restrictions, metricHash);
        string restrictionCheckHex = HashToHex(restrictionCheck);

        string[] splitter = { "\r\n" };
        string[] fileStrings = fileString.Split(splitter, StringSplitOptions.RemoveEmptyEntries);

        
        FileStream outFileStream = new FileStream(filename+".new", FileMode.Create);
        StreamWriter streamWriter = new StreamWriter(outFileStream);

        foreach (string lineString in fileStrings)
        {
          if (lineString.Contains("PasswordHash="))
            streamWriter.WriteLine("PasswordHash="+passwordHashHex);
          else if (lineString.Contains("Restrictions="))
            streamWriter.WriteLine("Restrictions="+restrictions);
          else if (lineString.Contains("RestrictionsCheck="))
            streamWriter.WriteLine("RestrictionsCheck="+restrictionCheckHex);
          else
            streamWriter.WriteLine(lineString);
        }

        streamWriter.Close();
        outFileStream.Close();
      }
    }

    private void CheckBox_CheckedChanged(object sender, EventArgs e)
    {
      restrictions = restrictions.Replace((sender as CheckBox).Text, "");

      if ((sender as CheckBox).Checked)
        restrictions += "|" + (sender as CheckBox).Text;

      // Tidy up.
      if (restrictions[0] == '|')
        restrictions = restrictions.Remove(0, 1);
      restrictions = restrictions.Replace("||", "|");
    }

    private void encryptButton_Click(object sender, EventArgs e)
    {
      OpenFileDialog openDialog = new OpenFileDialog();
      openDialog.Title = "Open PGM File";
      openDialog.Filter = "PGM Files|*.pgm";
      openDialog.AddExtension = true;
      openDialog.CheckFileExists = true;
      openDialog.CheckPathExists = true;
      if (openDialog.ShowDialog() == DialogResult.OK)
      {
        FileStream fileStream = new FileStream(openDialog.FileName, FileMode.Open);
        FileStream fileStreamZ = new FileStream(openDialog.FileName + ".z", FileMode.Create);

        RijndaelManaged rijndael = new RijndaelManaged();

        byte[] key = new byte[rijndael.BlockSize/8];
        for (int i = 0; i < Math.Min(key.Length, metricHash.Length); i++)
          key[i] = metricHash[i];

        byte[] iv = new byte[rijndael.BlockSize / 8];
        for (int i = 0; i < Math.Min(iv.Length, metricHash.Length); i++)
          iv[i] = metricHash[i];

        CryptoStream cryptoFileStreamZ = new CryptoStream(fileStreamZ, rijndael.CreateEncryptor(key, iv), CryptoStreamMode.Write);

        StreamReader fileStreamReader = new StreamReader(fileStream);
        StreamWriter cryptoFileStreamZWriter = new StreamWriter(cryptoFileStreamZ);

        cryptoFileStreamZWriter.Write(fileStreamReader.ReadToEnd());

        cryptoFileStreamZWriter.Close();
        
        fileStream.Close();
        fileStreamZ.Close();

        File.Delete(openDialog.FileName);
      }
    }

    private void decryptButton_Click(object sender, EventArgs e)
    {
      OpenFileDialog openDialog = new OpenFileDialog();
      openDialog.Title = "Open PGM.z File";
      openDialog.Filter = "PGM.z Files|*.pgm.z";
      openDialog.AddExtension = true;
      openDialog.CheckFileExists = true;
      openDialog.CheckPathExists = true;
      if (openDialog.ShowDialog() == DialogResult.OK)
      {
        FileStream fileStream = new FileStream(openDialog.FileName.Substring(0, openDialog.FileName.Length-2), FileMode.Create);
        FileStream fileStreamZ = new FileStream(openDialog.FileName, FileMode.Open);

        RijndaelManaged rijndael = new RijndaelManaged();

        byte[] key = new byte[rijndael.BlockSize / 8];
        for (int i = 0; i < Math.Min(key.Length, metricHash.Length); i++)
          key[i] = metricHash[i];

        byte[] iv = new byte[rijndael.BlockSize / 8];
        for (int i = 0; i < Math.Min(iv.Length, metricHash.Length); i++)
          iv[i] = metricHash[i];

        CryptoStream cryptoFileStreamZ = new CryptoStream(fileStreamZ, rijndael.CreateDecryptor(key, iv), CryptoStreamMode.Read);

        StreamWriter fileStreamWriter = new StreamWriter(fileStream);
        StreamReader cryptoFileStreamZReader = new StreamReader(cryptoFileStreamZ);

        fileStreamWriter.Write(cryptoFileStreamZReader.ReadToEnd());

        fileStreamWriter.Close();

        fileStream.Close();
        fileStreamZ.Close();

        File.Delete(openDialog.FileName);
      }
    }
  
  }
}