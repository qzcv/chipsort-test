#include "GRRreportWdg.h"
#include "GRRreportMod.h"
#include "ui_GRRreportWdg.h"
#include <QMessageBox>
#include <QInputDialog>

#define TOP_ITEM "Part"

#define IDLE "idle"
#define LOGGING "logging"
#define COMPLETED "completed"

GRRreportWdg::GRRreportWdg(int i) :
	ModSetWidget(),ui(new Ui::GRRreportWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
	m_topItem.clear();
	m_subItem.clear();
	sp_LSL.clear();
	sp_USL.clear();
}

GRRreportWdg::~GRRreportWdg()
{
	delete ui;
}

void GRRreportWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<GRRreportMod*>(module);
	m_param = m_module->m_param;
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void GRRreportWdg::showEvent(QShowEvent * event)
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

bool GRRreportWdg::eventFilter(QObject * watched, QEvent * event)
{
	QSpinBox *sp = qobject_cast<QSpinBox *>(watched);

	if (sp == ui->sp_inputUnits&&event->type() == QEvent::Wheel)
		return true;
	else if (sp == ui->sp_num&&event->type() == QEvent::Wheel)
		return true;
	else
		return QObject::eventFilter(watched, event);
}

void GRRreportWdg::le_textChanged(const QString &text)
{
	QLineEdit *le = qobject_cast<QLineEdit *>(sender());

	if (le == ui->le_lotName)
		m_param->GrrLotName = text;
	m_module->setParamChanged(ProductLevel);
}

void GRRreportWdg::sp_valueChanged(int val)
{
	QSpinBox *sp = qobject_cast<QSpinBox *>(sender());

	if (sp == ui->sp_inputUnits)
	{
		m_param->InputUnits = val;
		ui->lb_reportMode->setText(val == 1 ? QString("SPC") : QString("GRR"));
	}
	else if (sp == ui->sp_num)
	{
		m_param->modNum = val;
		m_module->updatePin();
		m_module->setParamChanged(StructLevel);

		while (m_param->itemNum.size() < m_param->modNum)
			m_param->itemNum.push_back(0);
		while (m_param->itemNum.size() > m_param->modNum)
			m_param->itemNum.pop_back();

		setTopItem();
	}
	m_module->setParamChanged(ProductLevel);
}

void GRRreportWdg::dsp_valueChanged(double val)
{
	QDoubleSpinBox *dsp = qobject_cast<QDoubleSpinBox *>(sender());

	for (auto i = 0; i < m_param->modNum; ++i)
		for (auto k = 0; k < m_param->itemNum[i]; ++k)
			if (sp_USL[i][k] == dsp)
				m_param->USL[i][k] = val;
			else if (sp_LSL[i][k] == dsp)
				m_param->LSL[i][k] = val;

	m_module->setParamChanged(ProductLevel);
}

void GRRreportWdg::bt_clicked()
{
	QPushButton *bt = qobject_cast<QPushButton *>(sender());
	if (ui->bt_genGRRreport == bt)
	{
		auto select = QMessageBox::information(this, tr("information"),
			tr("The default output is a csv file. Should it be changed to an html file (read-only)?"),
			QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		if (select == QMessageBox::Yes)
			m_module->genGrrReport_html();
		else if (select == QMessageBox::No)
			m_module->genGrrReport_xlsx();
	}
}

void GRRreportWdg::bt_toggled(bool checked)
{
	QPushButton *bt = qobject_cast<QPushButton *>(sender());
	if (ui->bt_ready == bt)
	{
		m_module->m_ready = checked;
		m_module->m_count = 0;

		ui->lb_detectCount->setText(QString("0/%1").arg(m_param->InputUnits * 9));
		ui->lb_logState->setText(IDLE);
	}
}

void GRRreportWdg::cb_toggled(bool checked)
{
	QCheckBox *cb = qobject_cast<QCheckBox *>(sender());

	if (cb == ui->cb_outPutAllMsg)
		m_param->IsOutAll = checked;
	else if (cb == ui->cb_useStdTol)
	{
		m_param->IsGrrByTol = checked;
		ui->tree_item->setColumnHidden(4, !checked);
		ui->tree_item->setColumnHidden(3, !checked);
	}
}

void GRRreportWdg::tree_itemChanged(QTreeWidgetItem *item, int colnum)
{
	if (colnum != 1 || m_topItem.contains(item))
		return;
	
	int i = -1, j = -1;
	for (auto k = 0; k < m_subItem.size(); ++k)
	{
		int idx = m_subItem[k].indexOf(item);
		if (idx != -1)
		{
			i = k;
			j = idx;
			break;
		}
	}

	if (i != -1 && j != -1)
	{
		m_param->enable[i][j] = item->checkState(1) == Qt::Checked ? 1 : 0;
		m_module->setParamChanged(ProductLevel);
	}
}

void GRRreportWdg::tree_itemDoubleClicked(QTreeWidgetItem *item, int colnum)
{
	if (colnum != 2 || m_topItem.contains(item))
		return;

	int i = -1, j = -1;
	for (auto k = 0; k < m_subItem.size(); ++k)
	{
		int idx = m_subItem[k].indexOf(item);
		if (idx != -1)
		{
			i = k;
			j = idx;
			break;
		}
	}

	if (i != -1 && j != -1)
	{
		QString text = item->text(2);
		bool ok = false;
		text = QInputDialog::getText(this, tr("Set Text"), tr("Text:"), QLineEdit::Normal, text, &ok);
		if (ok&& !text.isEmpty())
		{
			item->setText(2, text);
			m_param->itText[i][j] = text;
			m_module->setParamChanged(ProductLevel);
		}
	}
}

void GRRreportWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->sp_num, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_inputUnits, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));

	connectOrNot(link, ui->le_lotName, SIGNAL(textChanged(const QString &)), SLOT(le_textChanged(const QString &)));

	connectOrNot(link, ui->bt_genGRRreport, SIGNAL(clicked()), SLOT(bt_clicked()));

	connectOrNot(link, ui->bt_ready, SIGNAL(toggled(bool)), SLOT(bt_toggled(bool)));

	connectOrNot(link, ui->cb_useStdTol, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_outPutAllMsg, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));

	connectOrNot(link, ui->tree_item, SIGNAL(itemChanged(QTreeWidgetItem *, int)), SLOT(tree_itemChanged(QTreeWidgetItem *, int)));
	connectOrNot(link, ui->tree_item, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), SLOT(tree_itemDoubleClicked(QTreeWidgetItem *, int)));
}

void GRRreportWdg::iniUi()
{
	ui->sp_num->installEventFilter(this);
	ui->sp_inputUnits->installEventFilter(this);

	QStringList titles = { tr("Item"),tr("Enable"),tr("Text"),tr("LSL"),tr("USL") };
	ui->tree_item->setHeaderLabels(titles);
	ui->tree_item->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->tree_item->header()->setStretchLastSection(false);
}

void GRRreportWdg::setUiValue()
{
	if(!m_param)
		return;

	ui->sp_num->setVisible(m_widgetType != 0);
	ui->lb_num->setVisible(m_widgetType != 0);

	ui->sp_num->setValue(m_param->modNum);
	ui->sp_inputUnits->setValue(m_param->InputUnits);
	ui->le_lotName->setText(m_param->GrrLotName);
	ui->cb_outPutAllMsg->setChecked(m_param->IsOutAll);

	if (m_module->m_status == GRRreportMod::idle)
		ui->lb_logState->setText(IDLE);
	else if (m_module->m_status == GRRreportMod::logging)
		ui->lb_logState->setText(LOGGING);
	else if (m_module->m_status == GRRreportMod::completed)
		ui->lb_logState->setText(COMPLETED);

	ui->lb_detectCount->setText(QString("%1/%2").arg(m_module->m_count).arg(m_param->InputUnits * 9));
	ui->bt_genGRRreport->setEnabled(m_module->m_count == m_param->InputUnits * 9);

	setTopItem();
	setSubItem();
	ui->tree_item->setColumnHidden(4, !m_param->IsGrrByTol);
	ui->tree_item->setColumnHidden(3, !m_param->IsGrrByTol);
}

void GRRreportWdg::setTopItem()
{
	if (m_topItem.size() == m_param->modNum)
		return;

	while (m_topItem.size() > m_param->modNum)
	{
		int idx = m_topItem.size() - 1;
		QTreeWidgetItem *topIt = m_topItem.back();
		while (m_subItem[idx].size())
		{
			ui->tree_item->removeItemWidget(m_subItem[idx].back(), 3);
			ui->tree_item->removeItemWidget(m_subItem[idx].back(), 4);
			delete sp_LSL[idx].back();
			delete sp_USL[idx].back();
			sp_LSL[idx].pop_back();
			sp_USL[idx].pop_back();

			topIt->removeChild(m_subItem[idx].back());
			delete m_subItem[idx].back();
			m_subItem[idx].pop_back();
		}
		delete m_topItem.back();
		m_topItem.pop_back();
	}
	while (m_topItem.size() < m_param->modNum)
	{
		QTreeWidgetItem *topIt = new QTreeWidgetItem;
		topIt->setText(0, QString("%1%2").arg(TOP_ITEM).arg(m_topItem.size()));
		m_topItem.push_back(topIt);
		ui->tree_item->addTopLevelItem(topIt);

		m_subItem.push_back(QList<QTreeWidgetItem *>());
		sp_LSL.push_back(QList<QDoubleSpinBox *>());
		sp_USL.push_back(QList<QDoubleSpinBox *>());
	}

	while (m_param->enable.size() > m_param->modNum)
	{
		m_param->enable.pop_back();
		m_param->itText.pop_back();
		m_param->LSL.pop_back();
		m_param->USL.pop_back();
	}
	while (m_param->enable.size() < m_param->modNum)
	{
		m_param->enable.push_back(QVector<bool>());
		m_param->itText.push_back(QVector<QString>());
		m_param->LSL.push_back(QVector<double>());
		m_param->USL.push_back(QVector<double>());
	}
}

void GRRreportWdg::setSubItem()
{
	for (auto i = 0; i < m_param->modNum; ++i)
	{
		const QMap<QString, QVariant> *map = &(***m_module->p_it.at(i));

		if (needUpdateTreeItem(i))
		{
			while (m_subItem[i].size() > map->size())
			{
				ui->tree_item->removeItemWidget(m_subItem[i].back(), 3);
				ui->tree_item->removeItemWidget(m_subItem[i].back(), 4);
				delete sp_LSL[i].back();
				delete sp_USL[i].back();
				sp_LSL[i].pop_back();
				sp_USL[i].pop_back();

				m_topItem[i]->removeChild(m_subItem[i].back());
				delete m_subItem[i].back();
				m_subItem[i].pop_back();
			}
			while (m_subItem[i].size() < map->size())
			{
				QTreeWidgetItem *item = new QTreeWidgetItem(m_topItem[i]);
				m_subItem[i].push_back(item);

				QDoubleSpinBox *spUsl = new QDoubleSpinBox;
				QDoubleSpinBox *spLsl = new QDoubleSpinBox;
				sp_USL[i].push_back(spUsl);
				sp_LSL[i].push_back(spLsl);

				spUsl->setMinimum(-99);
				spLsl->setMinimum(-99);
				spUsl->setMaximum(99);
				spLsl->setMaximum(99);
				spUsl->setDecimals(3);
				spLsl->setDecimals(3);
				connect(spUsl, SIGNAL(valueChanged(double)), SLOT(dsp_valueChanged(double)));
				connect(spLsl, SIGNAL(valueChanged(double)), SLOT(dsp_valueChanged(double)));
			}
		}

		m_param->itemNum[i] = map->size();
		if (m_param->enable[i].size() != m_param->itemNum[i])
		{
			m_param->enable[i] = QVector<bool>(m_param->itemNum[i], 1);
			m_param->itText[i] = QVector<QString>(m_param->itemNum[i], QString(""));
			m_param->LSL[i] = QVector<double>(m_param->itemNum[i], 0.0);
			m_param->USL[i] = QVector<double>(m_param->itemNum[i], 1.0);
			int idx = 0;
			for (auto it = map->constBegin(); it != map->constEnd(); ++it, ++idx)
				m_param->itText[i][idx] = it.key();
		}

		int idx = 0;
		for (auto it = map->constBegin(); it != map->constEnd(); ++it, ++idx)
		{
			m_subItem[i][idx]->setText(0, it.key());
			m_subItem[i][idx]->setCheckState(1, m_param->enable[i][idx] ? Qt::Checked : Qt::Unchecked);
			m_subItem[i][idx]->setText(2, m_param->itText[i][idx]);

			sp_LSL[i][idx]->setValue(m_param->LSL[i][idx]);
			sp_USL[i][idx]->setValue(m_param->USL[i][idx]);
			ui->tree_item->setItemWidget(m_subItem[i][idx], 3, sp_LSL[i][idx]);
			ui->tree_item->setItemWidget(m_subItem[i][idx], 4, sp_USL[i][idx]);
		}
	}
}

bool GRRreportWdg::needUpdateTreeItem(int idx)
{
	if (m_subItem[idx].size() != (***(m_module->p_it.at(idx))).size())
		return true;
	else
	{
		if (m_subItem[idx].size() == 0)
			return false;
		else
		{
			auto subIt = m_subItem[idx].cbegin();
			auto pinIt = (***(m_module->p_it[idx])).cbegin();

			QString subText = (*subIt)->text(0);
			QString pinText = pinIt.key();

			if (subText == pinText)
				return false;
			else
				return true;
		}
	}
}

