using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;

/* Now, we can make this handle numeric values only in two ways - override OnTextChanged, and revert
 * the changes if we are not satisfied with the result. This will handly ctrl+v, etc. 
 * Alternatively, we could put something in to intercept keystrokes.
 * Finally, we could override OnValidating.
 * Lets try solution 1.
 */
namespace Reaction_Editor
{
    class NumericTextbox : TextBox
    {
        protected static Regex partialNumericRegex = new Regex(@"^(\+|-)?\d*(\.\d*)?(e(\+|-)?\d*)?$", RegexOptions.Compiled | RegexOptions.IgnoreCase | RegexOptions.ExplicitCapture);
        //protected static Regex completeNumericRegex = new Regex(@"^(+|-)?\d*(\.\d*)?)(\de(+|-)?\d+)?$", RegexOptions.Compiled | RegexOptions.IgnoreCase | RegexOptions.ExplicitCapture);
        ErrorProvider ep = new ErrorProvider();
        string oldString = "";

        protected override void OnTextChanged(EventArgs e)
        {
            if (!partialNumericRegex.Match(this.Text).Success)
            {
                int selStart = this.SelectionStart > oldString.Length ? oldString.Length : this.SelectionStart;
                int selLength = this.SelectionLength + selStart > oldString.Length ? oldString.Length - selStart : this.SelectionLength;
                this.Text = oldString;
                this.Select(selStart, selLength);
            }
            else
                oldString = this.Text;
        }

        protected override void OnValidating(System.ComponentModel.CancelEventArgs e)
        {
            double temp;
            e.Cancel = !double.TryParse(this.Text, out temp);
            if (e.Cancel)
                ep.SetError(this, "Invalid Number Format");
            else
                ep.SetError(this, "");
            base.OnValidating(e);
        }
    }
}