using System.ComponentModel;
using System.Globalization;
using System.Resources;

namespace WPFApplication1.Properties
{

    internal class Resources
    {

        private static CultureInfo _resCulture;
        private static ResourceManager _resMgr;

        [EditorBrowsable(EditorBrowsableState.Advanced)]
        public static CultureInfo Culture
        {
            get
            {
                return Resources._resCulture;
            }
            set
            {
                Resources._resCulture = value;
            }
        }

        [EditorBrowsable(EditorBrowsableState.Advanced)]
        public static ResourceManager ResourceManager
        {
            get
            {
                bool flag = Resources._resMgr != null;
                if (!flag)
                {
                    ResourceManager resourceManager1 = new ResourceManager("Resources\uFFFD", typeof(Resources).Assembly);
                    Resources._resMgr = resourceManager1;
                }
                return Resources._resMgr;
            }
        }

        internal Resources()
        {
        }

    } // class Resources

}

