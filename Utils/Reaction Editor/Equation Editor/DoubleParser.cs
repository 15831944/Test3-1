using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Reaction_Editor
{
    public static class DoubleParser
    {
        private static Regex doubleRegex = new Regex(
            @"^\s*(?<Whole>\d*)(\.(?<Fraction>\d*)?(e(?<Exponent>\d+)?\s*",
            RegexOptions.Compiled | RegexOptions.IgnoreCase);
        public static double Parse(string input)
        {
            Match m = doubleRegex.Match(input);
            if (!m.Success)
                throw new FormatException("Input string was not in a correct format");
            double ret = 0;
            if (m.Groups["Whole"].Length > 0)
                ret = (double)long.Parse(m.Groups["Whole"].Value);
            if (m.Groups["Fraction"].Success && m.Groups["Fraction"].Length > 0)
                ret += (double)long.Parse(m.Groups["Fraction"].Value) /
                    Math.Pow(10, m.Groups["Fraction"].Length);
            if (m.Groups["Exponent"].Success)
                ret *= Math.Pow(10, long.Parse(m.Groups["Exponent"].Value));
            return ret;
        }
    }
}
