#ifndef BON_BASE_CLASS_H
#define BON_BASE_CLASS_H


#include "..\TVCAS_B25\Exception.h"
#include "..\TVCAS_B25\TsUtilClass.h"


class __declspec(novtable) CBonBaseClass : public CBonErrorHandler
{
	CTracer *m_pTracer;
public:
	CBonBaseClass();
	virtual ~CBonBaseClass() = 0;
	virtual void SetTracer(CTracer *pTracer);
protected:
	void Trace(LPCTSTR pszOutput, ...);
};


#endif
