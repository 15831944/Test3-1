// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

using System;
using System.Reflection;
using System.Collections;
#if _DOTNET2
using System.Collections.Generic;
#endif

namespace VisualHint.SmartPropertyGrid
{
#if _DOTNET2
    internal class FieldInfoEnumComparer : IComparer<object>
#else
    internal class FieldInfoEnumComparer : IComparer
#endif
    {
        private Type _enumType;

        public FieldInfoEnumComparer(Type enumType)
        {
            _enumType = enumType;
        }

        public int Compare(object o1, object o2)
        {
            FieldInfo fi1 = o1 as FieldInfo;
            FieldInfo fi2 = o2 as FieldInfo;

            int intValue1 = Enum.Parse(_enumType, fi1.Name).GetHashCode();
            int intValue2 = Enum.Parse(_enumType, fi2.Name).GetHashCode();

            if (intValue1 < intValue2)
                return -1;
            else if (intValue1 > intValue2)
                return 1;
            else
                return 0;
        }
    }
}
