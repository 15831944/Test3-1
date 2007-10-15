using System.Data.OleDb;
using System;
using System.Data;
using System.Collections;


public class Database : IDisposable
{
    protected OleDbConnection m_Connection;
    protected string m_sTable;

    public Database(string location, string table)
    {
        m_Connection = new OleDbConnection(
            @"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + location);
        m_Connection.Open();
        m_sTable = table;
    }

    public OleDbConnection Connection { get { return m_Connection; } }

    public object GetFieldValue(string Name, string field)
    {
        OleDbCommand cmd = new OleDbCommand("SELECT " + field + " FROM " + m_sTable + " WHERE Name = '" + Name + "'", m_Connection);
        OleDbDataReader reader = cmd.ExecuteReader(CommandBehavior.SingleRow);
        reader.Read();
        object ret = reader.GetValue(reader.GetOrdinal(field));
        reader.Close();
        return ret;
    }

    public object[] GetFieldValues(string field)
    {
        OleDbCommand cmd = new OleDbCommand("SELECT " + field + " FROM " + m_sTable, m_Connection);
        OleDbDataReader reader = cmd.ExecuteReader();
        ArrayList ret = new ArrayList();
        while (reader.Read())
            ret.Add(reader.GetValue(reader.GetOrdinal(field)));
        reader.Close();
        return ret.ToArray();
    }

    public object[][] GetFieldValues(params string[] fields)
    {
        string csFields = "";
        foreach (string s in fields)
            csFields += s + ",";
        csFields = csFields.Substring(0, csFields.Length - 1);

        ArrayList[] ret = new ArrayList[fields.Length];
        for (int i = 0; i < ret.Length; i++)
            ret[i] = new ArrayList();

        OleDbCommand cmd = new OleDbCommand("SELECT " + csFields + " FROM " + m_sTable, m_Connection);
        OleDbDataReader reader = cmd.ExecuteReader();
        while (reader.Read())
            for (int i = 0; i < fields.Length; i++)
                ret[i].Add(reader.GetValue(reader.GetOrdinal(fields[i])));
        object[][] actualRet = new object[ret.Length][];
        for (int i = 0; i < ret.Length; i++)
            actualRet[i] = ret[i].ToArray();
        reader.Close();
        return actualRet;
    }

    public object[] GetSingleRow(string Command)
    {
        OleDbCommand cmd = new OleDbCommand(Command, m_Connection);
        OleDbDataReader reader = cmd.ExecuteReader(CommandBehavior.SingleRow);
        reader.Read();
        object[] buffer = new object[reader.FieldCount];
        reader.GetValues(buffer);
        reader.Close();
        return buffer;
    }

    #region IDisposable Members

    public void Dispose()
    {
        m_Connection.Close();
    }

    #endregion
}
