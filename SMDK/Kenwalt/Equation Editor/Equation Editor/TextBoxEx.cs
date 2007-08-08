using System.Windows.Forms;
using System.Threading;
using System;


namespace Reaction_Editor
{
    public class TextBoxEx : TextBox
    {
        protected System.Threading.Timer selectionChecker;
        public TextBoxEx()
        {
            selectionChecker = new System.Threading.Timer(new TimerCallback(checkSelection));
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
                selectionChecker.Dispose();
            base.Dispose(disposing);
        }

        private int oldSelectionStart = -1;
        private int oldSelectionLength = -1;
        int exCount = 0;
        private void checkSelection(object o)
        {
            try
            {
                if (this.InvokeRequired)
                {
                    this.BeginInvoke(new TimerCallback(checkSelection), o);
                    return;
                }
                if (oldSelectionLength != SelectionLength || oldSelectionStart != SelectionStart)
                {
                    oldSelectionLength = SelectionLength;
                    oldSelectionStart = SelectionStart;
                    if (SelectionChanged != null)
                        SelectionChanged(this, new EventArgs());
                }
            }
            catch
            {
                exCount++;
                if (exCount > 10)
                {
                    Console.WriteLine("More than 10 SelectionChecker errors. Disabling timer");
                    selectionChecker.Change(Timeout.Infinite, Timeout.Infinite);
                }
            }
        }

        public event EventHandler SelectionChanged;

        protected override void OnEnter(EventArgs e)
        {
            selectionChecker.Change(250, 250);
            base.OnEnter(e);
        }

        protected override void OnLeave(EventArgs e)
        {
            selectionChecker.Change(Timeout.Infinite, Timeout.Infinite);
            base.OnLeave(e);
        }

        public override int SelectionLength
        {
            get
            {
                return base.SelectionLength;
            }
            set
            {
                base.SelectionLength = value;
                checkSelection(null);
            }
        }

        public new void Select(int start, int length)
        {
            base.Select(start, length);
            checkSelection(null);
        }

        protected override void OnTextChanged(EventArgs e)
        {
            base.OnTextChanged(e);
            checkSelection(null);
        }
    }
}