// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.ComponentModel;
using Microsoft.Win32;


namespace MindFusion.FlowChartX
{
	public class RegistryLicenseProvider: LicenseProvider
	{
        public override License GetLicense(LicenseContext context, 
            Type type, object instance, bool allowExceptions)
		{
      return new RegLicense(this, type);
            if (context.UsageMode == LicenseUsageMode.Designtime)
			{
				RegistryKey licenseKey = Registry.LocalMachine.
					OpenSubKey("Software\\MindFusion Limited\\ComponentLicenses");

				if (licenseKey == null)
				{
					// the conrol might be installed on a 64-bit system
					licenseKey = Registry.LocalMachine.
						OpenSubKey("Software\\Wow6432Node\\MindFusion Limited\\ComponentLicenses");
				}

                if (licenseKey != null)
				{
					object keyVal = licenseKey.GetValue(type.FullName);
					if (keyVal != null)
					{
#if DEMO_VERSION
						return new RegLicense(this, type);
#else

#if FCNET_STD
						if (keyVal.ToString() == "fcxstd_lic_sG9VJ4sL")
							return new RegLicense(this, type);
#else
						if (keyVal.ToString() == "fcxpro_lic_FO7cGAk")
							return new RegLicense(this, type);
#endif
#endif
					}
				}

                if (allowExceptions)
				{
                    throw new LicenseException(type, instance,
                        "Couldn''t get design-time license for ''" + type.FullName + "''");
				}

				return null;
			}
            else
			{
				return new RuntimeRegLicense(this, type);
			}
		}

		private class RuntimeRegLicense : License
		{
			private RegistryLicenseProvider owner;
            private Type type;

			public RuntimeRegLicense(RegistryLicenseProvider owner, Type type)
			{
                this.owner = owner;
                this.type = type;
			}

            public override string LicenseKey
			{
                get { return type.FullName; }
			}

			public override void Dispose() {}
		}

        private class RegLicense : License
		{
            private RegistryLicenseProvider owner;
            private Type type;

            public RegLicense(RegistryLicenseProvider owner, Type type)
			{
                this.owner = owner;
                this.type = type;
			}

			public override string LicenseKey
			{
                get { return type.FullName; }
			}
            
			public override void Dispose() {}
		}      
	}
}
