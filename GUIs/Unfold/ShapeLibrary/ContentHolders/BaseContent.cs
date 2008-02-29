using System;
using System.Collections.Generic;
using System.Text;

namespace Orbifold.Unfold.ShapeLibrary
{
    public abstract class BaseContent
    {
        #region Fields
        private string mSubTitle;
        private string mTitle;
        private string mId;
        
        #endregion

        #region Properties
        public string SubTitle
        {
            get { return mSubTitle; }
            set { mSubTitle = value; }
        }


        public string Title
        {
            get { return mTitle; }
            set { mTitle = value; }
        }

        public string ID
        {
            get { return mId; }
            set { mId = value; }
        }

       
        #endregion

        #region Constructor
        public BaseContent(string id, string title, string subtitle)
        {
            this.mId = id;
            this.mTitle = title;
            this.mSubTitle = subtitle;
        }
        public BaseContent()
        { }
        #endregion
    }
}
