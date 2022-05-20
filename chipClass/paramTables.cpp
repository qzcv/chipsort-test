#include "paramTables.h"
#include <QHeaderView>

ParamTables::ParamTables(int Checknum, const vector<ParamType>& cbtype, bool labelOrCheck, bool withMinMax, int group0, int group1, QStringList cbName, QWidget *parent)
	: QWidget(parent)
{

	m_ParentTable = new QTableWidget(parent);
	m_labelOrCheck = labelOrCheck;
	m_withMinMax = withMinMax;
	m_loadingProject = false;
	iniData();
	m_ParentTable->setMinimumSize(parent->width(), 300);
	m_groupNumber[0] = group0;
	m_groupNumber[1] = group1;

	m_checkboxNum = Checknum;
	m_cbBox.clear();
	m_listHeader.clear();
	m_cbName.clear();
	m_cbName = cbName;
	m_ParentTable->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_ParentTable, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(ContextMenuRequested(const QPoint&)));
	QString actionText[2] = { tr("copy selected"),tr("copy all") };

	m_cMenu = new QMenu(this);
	for (int i = 0;i < 2;i++) {
		m_selAction[i] = new QAction(actionText[i], this);
		m_cMenu->addAction(m_selAction[i]);
		connect(m_selAction[i], SIGNAL(triggered()), this, SLOT(CopyActionTriggered()));
	}
	for (int i = 0;i < m_checkboxNum;i++)
	{
		QCheckBox *cbbox = new QCheckBox();
		m_cbBox.push_back(cbbox);
		ParamType tmptype = cbtype[(i > (cbtype.size() - 1) ? (cbtype.size() - 1) : i)];
		m_cbParamType.push_back(tmptype);
	}
	IniTableUI();
	IniTableSlots();
	connect(m_ParentTable, SIGNAL(cellChanged(int, int)), SLOT(tableWidget_cellChanged(int, int)));

}

void ParamTables::getcheckBoxPointer(const QCheckBox* ptr)
{
	ptr = m_cbBox[0];
}

void ParamTables::CopyActionTriggered()
{
	if (m_loadingProject)
		return;
	QAction* action = qobject_cast<QAction*>(sender());
	int level = 0;
	for (int i = 0;i < 2;i++) {
		if (action == m_selAction[i]) {
			level = i;
			break;
		}
	}
	QList<QTableWidgetSelectionRange> tmprange = m_ParentTable->selectedRanges();
	if (tmprange.size() == 0) {
		return;
	}

	int toprow, btmrow, lfcol, rtcol;
	if (level == 0) {  //复制选取
		toprow = tmprange[0].topRow();
		btmrow = tmprange[0].bottomRow();
		lfcol = tmprange[0].leftColumn();
		rtcol = tmprange[0].rightColumn();
		if (lfcol == 0 && rtcol == 0) {
			return;
		}
		emit CopyItems(false, toprow, btmrow - toprow + 1, lfcol - 1 - m_withMinMax, rtcol - lfcol + 1);
	}
	else {    //复制全部
		toprow = 0;
		btmrow = m_checkboxNum*(1 + m_withMinMax) - 1;
		lfcol = 1 + m_withMinMax;
		rtcol = m_groupNumber[0] + m_groupNumber[1] + m_withMinMax;
		emit CopyItems(true);
	}
	int grp0ColIndex = 1 + m_withMinMax;
	int grp1ColIndex = 1 + m_withMinMax + m_groupNumber[0];
	for (int r = toprow;r <= btmrow;r++) {
		int tmprt = ((rtcol + 1) > grp1ColIndex ? grp1ColIndex : (rtcol + 1));
		for (int c = lfcol;c < tmprt;c++) {
			if (lfcol >= tmprt) {
				break;
			}
			QString firstStr = m_ParentTable->item(r, grp0ColIndex)->text();
			if (firstStr.isEmpty()) {
				break;
			}
			if (c == grp0ColIndex)
				continue;
			m_loadingProject = true;
			m_ParentTable->item(r, c)->setText(firstStr);                //保护下当某一组管脚为0时应该怎么办？？？？？？？？？
			m_loadingProject = false;
		}
		int tmplf = (lfcol < grp1ColIndex ? grp1ColIndex : lfcol);
		for (int c = tmplf;c <= rtcol;c++) {
			if (rtcol < tmplf) {
				break;
			}
			QString firstStr = m_ParentTable->item(r, grp1ColIndex)->text();
			if (firstStr.isEmpty()) {
				firstStr = m_ParentTable->item(r, grp0ColIndex)->text();
				if (firstStr.isEmpty()) {
					break;
				}
			}
			m_loadingProject = true;
			m_ParentTable->item(r, c)->setText(firstStr);
			m_loadingProject = false;
		}
	}
	//log(2, tr("Set Widget Named ")+m_setDisableObject->objectName()+tr(" Permit Level ")+QString::number(level));
}

void ParamTables::iniData()
{
	m_templateName[0] = "L-";
	m_templateName[1] = "R-";
	m_headName = tr("Items");
	m_minRange = 0;
	m_maxRange = 999.999;
	m_decimals = 3;

}

ParamTables::~ParamTables()
{
	m_cbBox.clear();
	delete m_cMenu;
	for (int i = 0;i < 2;i++) {
		delete m_selAction[i];
	}
	m_ParentTable->clearContents();
	delete m_ParentTable;
}

void ParamTables::IniTableUI()
{
	m_ParentTable->setColumnCount(m_groupNumber[0] + m_groupNumber[1] + 1 + m_withMinMax);
	for (int i = 0;i < m_groupNumber[0] + m_groupNumber[1];i++) {
		m_ParentTable->setColumnWidth(i + 1 + m_withMinMax, 80);
	}
	if (m_withMinMax)
		m_ParentTable->setRowCount(m_checkboxNum * 2);
	else
		m_ParentTable->setRowCount(m_checkboxNum);


	m_ParentTable->horizontalHeader()->setVisible(true);
	m_ParentTable->verticalHeader()->setVisible(false);
	m_ParentTable->horizontalHeader()->setHighlightSections(false);
	//m_ParentTable->setShowGrid(false);
	setTemplateName(m_templateName[0], m_templateName[1]);
	setCheckboxName(m_cbName);
	//	m_ParentTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color: rgb(235, 235, 235);}");
	for (int i = 0;i < m_checkboxNum;i++)
	{
		if (m_withMinMax) {
			m_cbBox[i]->setChecked(false);
			m_ParentTable->setCellWidget(2 * i, 0, m_cbBox[i]);
			QTableWidgetItem *itemNull = new QTableWidgetItem();
			m_ParentTable->setItem(2 * i + 1, 0, itemNull);
			m_ParentTable->item(2 * i + 1, 0)->setFlags(Qt::NoItemFlags);
			if (m_labelOrCheck) {
				m_ParentTable->item(2 * i + 1, 0)->setBackgroundColor(QColor(223, 223, 223));
			}
			QTableWidgetItem *item = new QTableWidgetItem("Min");
			m_ParentTable->setItem(2 * i, 1, item);
			QTableWidgetItem *item1 = new QTableWidgetItem("Max");
			m_ParentTable->setItem(2 * i + 1, 1, item1);
			m_ParentTable->item(2 * i, 1)->setFlags(Qt::NoItemFlags);
			m_ParentTable->item(2 * i + 1, 1)->setFlags(Qt::NoItemFlags);
		}
		else {
			m_cbBox[i]->setChecked(false);
			m_ParentTable->setCellWidget(i, 0, m_cbBox[i]);
		}
		//m_ParentTable->setStyleSheet("QTableWidget::item{border:1px solid ;}");
		for (int j = 0;j < m_groupNumber[0] + m_groupNumber[1];j++) {
			QTableWidgetItem *item = new QTableWidgetItem();
			QTableWidgetItem *item1 = new QTableWidgetItem();
			if (m_withMinMax) {
				m_ParentTable->setItem(2 * i, j + 2, item);
				m_ParentTable->setItem(2 * i + 1, j + 2, item1);
				m_ParentTable->item(2 * i, j + 2)->setTextAlignment(Qt::AlignRight);
				m_ParentTable->item(2 * i + 1, j + 2)->setTextAlignment(Qt::AlignRight);
			}
			else {
				m_ParentTable->setItem(i, j + 1, item);
				m_ParentTable->item(i, j + 1)->setTextAlignment(Qt::AlignRight);
			}
		}
	}
	//setCellWidth(m_parentWid);
	//setCellHeight(m_parentHeight);
	//m_ParentTable->
	m_ParentTable->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	m_ParentTable->adjustSize();
	this->adjustSize();
}

void ParamTables::tableWidget_cellChanged(int row, int col)
{
	if (m_loadingProject)
		return;
	if (m_withMinMax && (col == 0 || col == 1) || !m_withMinMax && (col == 0)) return;
	QTableWidgetItem* item = m_ParentTable->item(row, col);
	if (!item)
		return;
	QString str = item->text();
	if (str.isEmpty()) {
		return;
	}
	//QRegExp rxs("^([0-9]|[1-9][0-9])+(\.\d{0,2})?$");
	//QRegExp rxs("^([0-9]|[1-9][0-9]|[1-2][0-9][0-9])+(\.\d{1,2})?$");
	QRegExp rxs("^(-)?(0|[1-9][0-9]?[0-9]?)(\.[0-9]{0,10})?$");
	//QRegExp rxs("^([1-9][0-9]*)$"); 
	bool enterok = true;
	double tmpval = str.toDouble();
	double tmpRange = m_minRange;
	if (!rxs.exactMatch(str)) {
		enterok = false;
		QMessageBox::about(this, tr("waring"), tr("Please enter the correct number"));
	}
	else {
		if (tmpval > m_maxRange || tmpval < m_minRange) {
			if (tmpval > m_maxRange) {
				tmpRange = m_maxRange;
			}
			enterok = false;
			QMessageBox::about(this, tr("waring"), tr("Please enter the number in the given range "));
		}
	}
	int grpIndex, leadIdx, checkIndex;
	bool minOrMax;
	if (m_withMinMax) {
		if (m_groupNumber[0] != 0) {
			grpIndex = ((col - 2) / m_groupNumber[0]) > 0 ? 1 : 0;
		}
		else {
			grpIndex = 1;
		}
		leadIdx = (col - 2) - grpIndex*m_groupNumber[0];
		checkIndex = row / 2;
		minOrMax = (row % 2) == 1;
	}
	else {
		if (m_groupNumber[0] != 0) {
			grpIndex = ((col - 1) / m_groupNumber[0]) > 0 ? 1 : 0;
		}
		else {
			grpIndex = 1;
		}
		leadIdx = (col - 1) - grpIndex*m_groupNumber[0];
		checkIndex = row;
		minOrMax = 0;
	}
	if (!enterok) {
		tmpval = tmpRange;
	}
	QString valstr;
	if (m_decimals > 0) {
		valstr = QString::number(tmpval, 'f', m_decimals);
	}
	else if (m_decimals == 0) {
		valstr = QString::number((int)(tmpval + 0.5));
	}

	m_loadingProject = true;
	item->setText(valstr);
	if (m_cbParamType[checkIndex] == groupParam) {
		for (int i = 0;i < m_groupNumber[grpIndex];i++) {
			m_ParentTable->item((1 + m_withMinMax)*checkIndex + (int)minOrMax*m_withMinMax, m_withMinMax + 1 + m_groupNumber[0] * grpIndex + i)->setText(valstr);
		}
	}
	else if (m_cbParamType[checkIndex] == sameParam) {
		for (int i = 0;i < m_groupNumber[0] + m_groupNumber[1];i++) {
			m_ParentTable->item((1 + m_withMinMax)*checkIndex + (int)minOrMax*m_withMinMax, m_withMinMax + 1 + i)->setText(valstr);
		}
	}
	m_loadingProject = false;
	emit CellChanged(checkIndex, grpIndex, leadIdx, minOrMax, item->text().toDouble());
}

void ParamTables::getValue(int checkIdx, int grpIdx, int leadIdx, bool minOrMax, double& val)
{
	int colIdx = 1 + m_withMinMax + m_groupNumber[0] * grpIdx + leadIdx;
	int rowIdx = (1 + m_withMinMax)*checkIdx + (int)minOrMax*m_withMinMax;
	QString valstr = m_ParentTable->item(rowIdx, colIdx)->text();
	val = valstr.toDouble();
}

void ParamTables::setValue(int checkIdx, int grpIdx, int leadIdx, bool minOrMax, double val)
{
	int colIdx = 1 + m_withMinMax + m_groupNumber[0] * grpIdx + leadIdx;
	double trueVal = val;
	if (val > m_maxRange) {
		trueVal = m_maxRange;
	}
	else if (val < m_minRange) {
		trueVal = m_minRange;
	}
	m_loadingProject = true;
	QString valstr;
	if (m_decimals > 0) {
		valstr = QString::number(trueVal, 'f', m_decimals);
	}
	else if (m_decimals == 0) {
		valstr = QString::number((int)(trueVal + 0.5));
	}
	if (m_cbParamType[checkIdx] == singleParam) {
		m_ParentTable->item((1 + m_withMinMax)*checkIdx + (int)minOrMax*m_withMinMax, colIdx)->setText(valstr);
	}
	else if (m_cbParamType[checkIdx] == groupParam) {
		for (int i = 0;i < m_groupNumber[grpIdx];i++) {
			m_ParentTable->item((1 + m_withMinMax)*checkIdx + (int)minOrMax*m_withMinMax, m_withMinMax + 1 + m_groupNumber[0] * grpIdx + i)->setText(valstr);
		}
	}
	else if (m_cbParamType[checkIdx] == sameParam) {
		for (int i = 0;i < m_groupNumber[0] + m_groupNumber[1];i++) {
			m_ParentTable->item((1 + m_withMinMax)*checkIdx + (int)minOrMax*m_withMinMax, m_withMinMax + 1 + i)->setText(valstr);
		}
	}
	m_loadingProject = false;
}

void ParamTables::setParamType(int checkIdx, ParamType ptype)
{
	m_cbParamType[checkIdx] = ptype;
}

void ParamTables::ContextMenuRequested(const QPoint & pos)
{
	m_cMenu->exec(QCursor::pos());
}

void ParamTables::IniTableSlots()
{
	for (int i = 0;i < m_checkboxNum;i++)
	{
		connect(m_cbBox[i], SIGNAL(toggled(bool)), SLOT(Cb_Toggled(bool)));
	}
}

void ParamTables::Cb_Toggled(bool val)
{
	QCheckBox* cbbox = qobject_cast<QCheckBox*>(sender());
	for (int i = 0;i < m_checkboxNum;i++)
	{
		if (cbbox == m_cbBox[i]) {
			emit checkBoxToggled(i, val);
			return;
		}
	}
}

QTableWidget * ParamTables::getTable()
{
	return m_ParentTable;
}

void ParamTables::setGroupLeadNumber(int Group0, int Group1)
{
	bool changed = false;
	if ((Group0 + Group1) == 0)
		return;
	if (Group0 != m_groupNumber[0] || Group1 != m_groupNumber[1]) {
		changed = true;
	}
	if (changed) {  //应将下部整理为独立的部分->resetLeadNumber();
		if ((Group0 + Group1) < (m_groupNumber[0] + m_groupNumber[1])) {   //移除多余的列数，释放内存
			for (int i = Group0 + Group1 + 1 + m_withMinMax;i < (m_groupNumber[0] + m_groupNumber[1] + 1 + m_withMinMax);i++) {
				m_ParentTable->removeColumn(i);
			}
		}
		m_ParentTable->setColumnCount(Group0 + Group1 + 1 + m_withMinMax);  //是否会释放内存？
		for (int i = 0;i < m_groupNumber[0] + m_groupNumber[1];i++) {
			m_ParentTable->setColumnWidth(i + 1 + m_withMinMax, 80);
		}
		if ((Group0 + Group1) > (m_groupNumber[0] + m_groupNumber[1])) {  //增加多余的列数
			for (int i = 0;i < m_checkboxNum;i++) {
				for (int j = m_groupNumber[0] + m_groupNumber[1] + 1 + m_withMinMax;j < Group0 + Group1 + 1 + m_withMinMax;j++) {
					QTableWidgetItem *item = new QTableWidgetItem();
					QTableWidgetItem *item1 = new QTableWidgetItem();
					if (m_withMinMax) {
						m_ParentTable->setItem(2 * i, j, item);
						m_ParentTable->setItem(2 * i + 1, j, item1);
						m_ParentTable->item(2 * i, j)->setTextAlignment(Qt::AlignRight);
						m_ParentTable->item(2 * i + 1, j)->setTextAlignment(Qt::AlignRight);
					}
					else {
						m_ParentTable->setItem(i, j, item);
						m_ParentTable->item(i, j)->setTextAlignment(Qt::AlignRight);
					}
				}
			}
		}
		m_groupNumber[0] = Group0;
		m_groupNumber[1] = Group1;
		iniTabHeaderLabels();
		resetLeadNumber();
	}
}

void ParamTables::resetLeadNumber()
{

}

void ParamTables::setTemplateName(QString group0, QString group1)
{
	m_templateName[0] = group0;
	m_templateName[1] = group1;
	iniTabHeaderLabels();
}

void ParamTables::iniTabHeaderLabels()
{
	m_listHeader.clear();
	m_listHeader << m_headName;
	if (m_withMinMax)
		m_listHeader << " ";
	for (int g = 0;g < 2;g++) {
		for (int i = 0;i < m_groupNumber[g];i++) {
			QString tmpstr;
			tmpstr = m_templateName[g] + tr("%1").arg(i + 1);
			m_listHeader << tmpstr;
		}
	}
	m_ParentTable->setHorizontalHeaderLabels(m_listHeader);
}

void ParamTables::setTableWidth(int Wid)
{
	m_ParentTable->setFixedWidth(Wid);
}

void ParamTables::setTableHeight(int Height)
{
	m_ParentTable->setFixedHeight(Height);
}

void ParamTables::setCheckboxName(QStringList Cbname)
{
	m_cbName = Cbname;
	int diffnum = m_checkboxNum - m_cbName.size();
	for (int i = 0;i < diffnum;i++)
	{
		m_cbName.push_back("NULL");
	}
	for (int i = 0;i < m_checkboxNum;i++)
	{
		m_cbBox[i]->setText(m_cbName[i]);
		m_cbBox[i]->setLayoutDirection(Qt::RightToLeft);
		//m_cbBox[i]->setStyleSheet("background-color: rgb(223, 223, 223);");
		QString stylestr;
		if (m_labelOrCheck) {
			stylestr = "background-color: rgb(223, 223, 223);";
		}
		else {
			stylestr = "QCheckBox::indicator{width: 0px};QCheckBox::background{color: rgb(223, 223, 223)};";
			//m_ParentTable->setStyleSheet("QTableWidget::item{border:1px solid ;}");
		}
		m_cbBox[i]->setStyleSheet(stylestr);
	}
}

void ParamTables::getCheckBoxVal(int checkIdx, int& checkVal)
{
	checkVal = m_cbBox[checkIdx]->isChecked();
}

void ParamTables::getCheckBoxName(int checkIdx, QString& checkName)
{
	checkName = m_cbName[checkIdx];
}

void ParamTables::setHeaderName(QStringList Headername)
{
	m_listHeader = Headername;
	m_ParentTable->setHorizontalHeaderLabels(m_listHeader);
}

void ParamTables::setHeadName(QString headname)
{
	m_headName = headname;
	iniTabHeaderLabels();
}

void ParamTables::setValueRange(double LSL, double USL, int decimals)
{
	if (USL >= LSL) {
		m_minRange = LSL;
		m_maxRange = USL;
		m_decimals = decimals;
	}
}






