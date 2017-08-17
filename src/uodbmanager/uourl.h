#ifndef UOURL_H
#define UOURL_H
#include <QString>
#include "uodefines.h"
class uoElementData;

///\breaf - класс предназначен для хранения описания объекта.
class uoUrl
{
public:
    explicit uoUrl();
    uoUrl(QString url);
    uoUrl(QString table, int elem, uoFormType formtype,  int parent);
    uoUrl(QString table, int elem, int parent);
    uoUrl(uoElementData* el);

    uoFormType formType() const {return m_formType; }
    QString toString(bool withIID = false) const ;
    QString table() const { return m_table ;}
    int     element() const { return m_elem ;}
    int     parent()  const { return m_elemParent ;}
    bool    selection() const { return m_selection; }
    void    setSelection(bool sele){ m_selection = sele; }
    QString ovnerForm() const { return m_ovnerUrlForm; }
    void    setOvnerForm(QString oForm) { m_ovnerUrlForm = oForm; }
    void    setReadOnly(bool readOnly) { m_readOnly = readOnly; }
    void    setWithElem(bool with) { m_withElem = with; }

    bool    isFolder() { return (m_formType == uoFormT_Group) ? true : false; }
    bool    isElement() { return (m_formType == uoFormT_Elem) ? true : false; }
    bool    isList() { return (m_formType == uoFormT_List) ? true : false; }
    bool    isReadOnly() { return (m_readOnly) ? true : false; }
    bool    isWithElem() const { return m_withElem; }
private:
    void initEmpty();
    void parseUrl(QString url);
    QString    m_ovnerUrlForm; //форма из которой был вызван элемент и которую надо проинформировать об изменении.
    QString    m_table;
    uoUrlType  m_type;
    uoFormType m_formType;
    int        m_elem;
    int        m_elemParent;
    bool       m_selection; /// true = открыт подбор
    bool       m_withElem; /// true = вместе с элементами
    bool       m_readOnly; /// true = открыт подбор
    long       m_internalID;
};

#endif // UOURL_H
