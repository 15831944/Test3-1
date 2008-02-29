using System;
using System.Collections.Generic;
using System.Text;
using System.Collections.ObjectModel;
namespace Orbifold.Unfold.ShapeLibrary  
{
    public class ClassCategory
    {
        private string mCategoryName;
        public string CategoryName
        {
            get { return mCategoryName; }
            set { mCategoryName = value; }
        }
        private ObservableCollection<ClassMember> mMembers;
        public ObservableCollection<ClassMember> Members
        {
            get
            {
                if (mMembers == null)
                    mMembers = new ObservableCollection<ClassMember>();
                return mMembers;
            }
        }

        public ClassCategory()
        {
        }

        public ClassCategory(string categoryName, ObservableCollection<ClassMember> members)
        {
            mCategoryName = categoryName;
            mMembers = members;
        }
    }
    public class ClassMember
    {
        private string mName;
        public string Name
        {
            get { return mName; }
            set { mName = value; }
        }
        public ClassMember(){}
        public ClassMember(string name, string icon)
        {
            mName = name;
            mImage = icon;
        }
        private string mImage = "PublicMethod";

        public string Image
        {
            get { return "images/" + mImage + ".ico"; }
            set { mImage = value; }
        }



    }
}
