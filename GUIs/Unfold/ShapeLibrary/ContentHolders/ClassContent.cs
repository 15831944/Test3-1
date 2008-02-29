using System;
using System.Collections.Generic;
using System.Text;

namespace Orbifold.Unfold.ShapeLibrary
{
    /// <summary>
    /// A meme or thought is just an encapsulation of a title, a subtitle and a bunch of text or description.
    /// </summary>
    public sealed class ClassContent : BaseContent
    {
        #region Fields
        /// <summary>
        /// The text
        /// </summary>
        private MemberContent mMembers = new MemberContent();
        #endregion

        #region Properties
        /// <summary>
        /// Gets or sets the members of the class.
        /// </summary>
        public MemberContent Members
        {
            get { return mMembers; }
            set { mMembers = value; }
        }
        #endregion

        #region Constructor
        /// <summary>
        /// Extended constructor
        /// </summary>
        /// <param name="id"></param>
        /// <param name="title"></param>
        /// <param name="subtitle"></param>
        /// <param name="description"></param>
        public ClassContent(string id, string title, string subtitle, MemberContent members) : base(id,title, subtitle)
        {            
            this.mMembers = members;
        }
        /// <summary>
        /// Default constructor
        /// </summary>
        public ClassContent()
        {
        
        }
        #endregion
    }
}
