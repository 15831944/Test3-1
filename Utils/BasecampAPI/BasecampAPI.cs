using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.IO;
using System.Xml;

namespace BasecampAPI
{
  public class API
  {
    string site = null;
    string username = null;
    string password = null;

    HttpStatusCode errorCode;
    string errorDescription = null;


   public API(string site, string username, string password)
    {
      this.site = site;
      this.username = username;
      this.password = password;
    }

    public Dictionary<string, int> GetProjectList()
    {
      Dictionary<string, int> projects = new Dictionary<string, int>();

      errorDescription = null;

      HttpWebRequest request = WebRequest.Create("https://" + site + "/project/list") as HttpWebRequest;
      request.Credentials = new NetworkCredential(username, password);
      request.ContentType = "application/xml";
      request.Accept = "application/xml";

      HttpWebResponse response = null;
      try
      {
        response = request.GetResponse() as HttpWebResponse;
        StreamReader responseReader = new StreamReader(response.GetResponseStream());

        XmlDocument xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(responseReader.ReadToEnd());

        XmlNode node = xmlDoc.SelectSingleNode("projects");
        foreach (XmlNode innerNode in node.SelectNodes("project"))
        {
          projects.Add(innerNode.SelectSingleNode("name").InnerText,
                       int.Parse(innerNode.SelectSingleNode("id").InnerText));
        }		
      }
      catch (WebException e)
      {
        if (e.Response != null)
        {
          HttpWebResponse errorResponse = e.Response as HttpWebResponse;

          errorDescription = errorResponse.StatusDescription;
          errorCode = errorResponse.StatusCode;
        }
        return null;
      }
      finally
      {
        if (response != null) { response.Close(); }
      }

      errorCode = HttpStatusCode.Accepted;
      return projects;
    }

    public Dictionary<string, int> GetCompanyList()
    {
      Dictionary<string, int> companies = new Dictionary<string, int>();

      errorDescription = null;

      HttpWebRequest request = WebRequest.Create("https://" + site + "/contacts/companies") as HttpWebRequest;
      request.Credentials = new NetworkCredential(username, password);
      request.ContentType = "application/xml";
      request.Accept = "application/xml";

      HttpWebResponse response = null;
      try
      {
        response = request.GetResponse() as HttpWebResponse;
        StreamReader responseReader = new StreamReader(response.GetResponseStream());

        XmlDocument xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(responseReader.ReadToEnd());

        XmlNode node = xmlDoc.SelectSingleNode("companies");
        foreach (XmlNode innerNode in node.SelectNodes("company"))
        {
          companies.Add(innerNode.SelectSingleNode("name").InnerText,
                       int.Parse(innerNode.SelectSingleNode("id").InnerText));
        }
      }
      catch (WebException e)
      {
        if (e.Response != null)
        {
          HttpWebResponse errorResponse = e.Response as HttpWebResponse;

          errorDescription = errorResponse.StatusDescription;
          errorCode = errorResponse.StatusCode;
        }
        return null;
      }
      finally
      {
        if (response != null) { response.Close(); }
      }

      errorCode = HttpStatusCode.Accepted;
      return companies;
    }

    public Dictionary<string, int> GetCategoryList(int projectID)
    {
      Dictionary<string, int> categories = new Dictionary<string, int>();

      errorDescription = null;

      HttpWebRequest request = WebRequest.Create("https://" + site + "/projects/" + projectID + "/post_categories") as HttpWebRequest;
      request.Credentials = new NetworkCredential(username, password);
      request.ContentType = "application/xml";
      request.Accept = "application/xml";

      HttpWebResponse response = null;
      try
      {
        response = request.GetResponse() as HttpWebResponse;
        StreamReader responseReader = new StreamReader(response.GetResponseStream());

        XmlDocument xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(responseReader.ReadToEnd());

        XmlNode node = xmlDoc.SelectSingleNode("post-categories");
        foreach (XmlNode innerNode in node.SelectNodes("post-category"))
        {
          categories.Add(innerNode.SelectSingleNode("name").InnerText,
                       int.Parse(innerNode.SelectSingleNode("id").InnerText));
        }
      }
      catch (WebException e)
      {
        if (e.Response != null)
        {
          HttpWebResponse errorResponse = e.Response as HttpWebResponse;

          errorDescription = errorResponse.StatusDescription;
          errorCode = errorResponse.StatusCode;
        }
        return null;
      }
      finally
      {
        if (response != null) { response.Close(); }
      }

      errorCode = HttpStatusCode.Accepted;
      return categories;
    }

    public Dictionary<string, int> GetPersonList(int projectID, int companyID)
    {
      Dictionary<string, int> people = new Dictionary<string, int>();

      errorDescription = null;

      HttpWebRequest request = WebRequest.Create("https://" + site + "/projects/" + projectID + "/contacts/people/" + companyID) as HttpWebRequest;
      request.Credentials = new NetworkCredential(username, password);
      request.ContentType = "application/xml";
      request.Accept = "application/xml";

      HttpWebResponse response = null;
      try
      {
        response = request.GetResponse() as HttpWebResponse;
        StreamReader responseReader = new StreamReader(response.GetResponseStream());

        XmlDocument xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(responseReader.ReadToEnd());

        XmlNode node = xmlDoc.SelectSingleNode("people");
        foreach (XmlNode innerNode in node.SelectNodes("person"))
        {
          people.Add(innerNode.SelectSingleNode("user-name").InnerText,
                     int.Parse(innerNode.SelectSingleNode("id").InnerText));
        }
      }
      catch (WebException e)
      {
        if (e.Response != null)
        {
          HttpWebResponse errorResponse = e.Response as HttpWebResponse;

          errorDescription = errorResponse.StatusDescription;
          errorCode = errorResponse.StatusCode;
        }
        return null;
      }
      finally
      {
        if (response != null) { response.Close(); }
      }

      errorCode = HttpStatusCode.Accepted;
      return people;
    }

    public string UploadFile(string fileName)
    {
      string fileID = null;

      HttpWebRequest request = WebRequest.Create("https://" + site + "/upload") as HttpWebRequest;
      request.Credentials = new NetworkCredential(username, password);
      request.Method = "POST";
      request.ContentType = "application/octet-stream";
      request.Accept = "application/xml";

      FileInfo fileInfo = new FileInfo(fileName);
      FileStream file = new FileStream(fileName, FileMode.Open);
      byte[] byteData = new byte[fileInfo.Length];
      file.Read(byteData, 0, byteData.Length);
      file.Close();

      request.ContentLength = byteData.Length;
      request.GetRequestStream().Write(byteData, 0, byteData.Length);

      HttpWebResponse response = null;
      try
      {
        response = request.GetResponse() as HttpWebResponse;
        StreamReader responseReader = new StreamReader(response.GetResponseStream());

        XmlDocument xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(responseReader.ReadToEnd());

        fileID = xmlDoc.SelectSingleNode("upload").SelectSingleNode("id").InnerText;
      }
      catch (WebException e)
      {
        if (e.Response != null)
        {
          HttpWebResponse errorResponse = e.Response as HttpWebResponse;

          errorDescription = errorResponse.StatusDescription;
          errorCode = errorResponse.StatusCode;
        }
        return null;
      }
      finally
      {
        if (response != null) { response.Close(); }
      }

      errorCode = HttpStatusCode.Accepted;
      
      return fileID;
    }

    public void CreateMessage(int projectID, int categoryID, int[] notificationIDList, string fileName, string description, string name, string fileID)
    {
      HttpWebRequest request = WebRequest.Create("https://" + site + "/projects/"+projectID+"/msg/create") as HttpWebRequest;
      //HttpWebRequest request = WebRequest.Create("https://kenwalt.updatelog.com/projects/351146/msg/create") as HttpWebRequest;
      request.Method = "POST";
      request.Credentials = new NetworkCredential(username, password);
      request.ContentType = "application/xml";
      request.Accept = "application/xml";

      if (description.Length == 0)
        description = fileName;

      if (name.Length == 0)
        name = fileName;

      string requestString =
        "<request>" +
          "<post>" +
            "<category-id>" +
              categoryID +
            "</category-id>" +
            "<title>" +
              fileName +
            "</title>" +
            "<body>" +
              description +
            "</body>" +
          "</post>";

      foreach (int notifyID in notificationIDList)
      {
        requestString +=
          "<notify>" +
            notifyID +
          "</notify>";
      }
      requestString +=          
          "<attachments>" +
            "<name>" +
              name +
            "</name>" +
            "<file>" +
              "<file>" +
                fileID +
              "</file>" +
              "<content-type>" +
                "application/octet-stream" +
              "</content-type>" +
              "<original_filename>" +
                fileName +
              "</original_filename>" +
            "</file>" +
          "</attachments>" +
        "</request>";
      
      byte[] byteData = UTF8Encoding.UTF8.GetBytes(requestString);

      request.ContentLength = byteData.Length;
      request.GetRequestStream().Write(byteData, 0, byteData.Length);

      HttpWebResponse response = null;
      try
      {
        response = request.GetResponse() as HttpWebResponse;
        StreamReader responseReader = new StreamReader(response.GetResponseStream());

        XmlDocument xmlDoc = new XmlDocument();
        xmlDoc.LoadXml(responseReader.ReadToEnd());
      }
      catch (WebException e)
      {
        if (e.Response != null)
        {
          HttpWebResponse errorResponse = e.Response as HttpWebResponse;

          errorDescription = errorResponse.StatusDescription;
          errorCode = errorResponse.StatusCode;
        }
      }
      finally
      {
        if (response != null) { response.Close(); }
      }

      errorCode = HttpStatusCode.Accepted;
    }
  }
}
