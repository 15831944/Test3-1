// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard

#import "C:\\GorstWork\\NET Projects\\OGLViewer\\debug\\OGLViewer.exe" no_namespace
// COGLViewer wrapper class

class COGLViewer : public COleDispatchDriver
{
public:
	COGLViewer(){} // Calls COleDispatchDriver default constructor
	COGLViewer(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	COGLViewer(const COGLViewer& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// Attributes
public:

	// Operations
public:


	// IOGLViewer methods
public:
	void SetSize(double dWidth, double dHeight)
	{
		static BYTE parms[] = VTS_R8 VTS_R8 ;
		InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, parms, dWidth, dHeight);
	}
	void AddDrawingPacket(short nMode, VARIANT * vaPars)
	{
		static BYTE parms[] = VTS_I2 VTS_PVARIANT ;
		InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nMode, vaPars);
	}
	void AddDrawingPacket2(short nMode, LPUNKNOWN pUnk)
	{
		static BYTE parms[] = VTS_I2 VTS_UNKNOWN ;
		InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nMode, pUnk);
	}

	// IOGLViewer properties
public:

};
