using System;
using System.Collections.Generic;
using System.Text;

namespace Orbifold.Unfold.ShapeLibrary
{
    /// <summary>
    /// A meme or thought is just an encapsulation of a title, a subtitle and a bunch of text or description.
    /// </summary>
    public sealed class MemeContent : BaseContent
    {
        #region Fields
        /// <summary>
        /// The text
        /// </summary>
        private string mText;
        #endregion

        #region Properties
        /// <summary>
        /// Gets or sets the text or description of the meme.
        /// </summary>
        public string Text
        {
            get { return mText; }
            set { mText = value; }
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
        public MemeContent(string id, string title, string subtitle, string text) : base(id,title, subtitle)
        {            
            this.mText = text;
        }
        /// <summary>
        /// Default constructor
        /// </summary>
        public MemeContent()
        {
        
        }
        #endregion
    }
}
