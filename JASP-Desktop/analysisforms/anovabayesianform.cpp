//
// Copyright (C) 2013-2016 University of Amsterdam
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
//

#include "anovabayesianform.h"
#include "ui_anovabayesianform.h"

AnovaBayesianForm::AnovaBayesianForm(QWidget *parent) :
	AnalysisForm("AnovaBayesianForm", parent),
	ui(new Ui::AnovaBayesianForm)
{
	ui->setupUi(this);

	ui->listAvailableFields->setModel(&_availableVariablesModel);

	_dependentListModel = new TableModelVariablesAssigned(this);
	_dependentListModel->setSource(&_availableVariablesModel);
	_dependentListModel->setVariableTypesSuggested(Column::ColumnTypeScale);
	_dependentListModel->setVariableTypesAllowed(Column::ColumnTypeScale | Column::ColumnTypeOrdinal | Column::ColumnTypeNominal);
	ui->dependent->setModel(_dependentListModel);

	_fixedFactorsListModel = new TableModelVariablesAssigned(this);
	_fixedFactorsListModel->setSource(&_availableVariablesModel);
	_fixedFactorsListModel->setVariableTypesSuggested(Column::ColumnTypeNominal | Column::ColumnTypeOrdinal);
	ui->fixedFactors->setModel(_fixedFactorsListModel);

	_randomFactorsListModel = new TableModelVariablesAssigned(this);
	_randomFactorsListModel->setSource(&_availableVariablesModel);
	_randomFactorsListModel->setVariableTypesSuggested(Column::ColumnTypeNominal | Column::ColumnTypeOrdinal);
	ui->randomFactors->setModel(_randomFactorsListModel);

	ui->buttonAssignDependent->setSourceAndTarget(ui->listAvailableFields, ui->dependent);
	ui->buttonAssignFixed->setSourceAndTarget(ui->listAvailableFields, ui->fixedFactors);
	ui->buttonAssignRandom->setSourceAndTarget(ui->listAvailableFields, ui->randomFactors);

	_anovaModel = new TableModelAnovaModel(this);
	_anovaModel->setPiecesCanBeAssigned(false);
	ui->modelTerms->setModel(_anovaModel);
	ui->modelTerms->hide();

	ui->advancedOptions->hide();

	ui->priorFixedEffects->setLabel("R-scale fixed effects");
	ui->priorRandomEffects->setLabel("R-scale random effects");

	connect(_fixedFactorsListModel, SIGNAL(assignmentsChanging()), this, SLOT(factorsChanging()));
	connect(_fixedFactorsListModel, SIGNAL(assignmentsChanged()), this, SLOT(factorsChanged()));
	connect(_fixedFactorsListModel, SIGNAL(assignedTo(Terms)), _anovaModel, SLOT(addFixedFactors(Terms)));
	connect(_fixedFactorsListModel, SIGNAL(unassigned(Terms)), _anovaModel, SLOT(removeVariables(Terms)));

	connect(_randomFactorsListModel, SIGNAL(assignmentsChanging()), this, SLOT(factorsChanging()));
	connect(_randomFactorsListModel, SIGNAL(assignmentsChanged()), this, SLOT(factorsChanged()));
	connect(_randomFactorsListModel, SIGNAL(assignedTo(Terms)), _anovaModel, SLOT(addRandomFactors(Terms)));
	connect(_randomFactorsListModel, SIGNAL(unassigned(Terms)), _anovaModel, SLOT(removeVariables(Terms)));

#ifdef QT_DEBUG
	ui->groupBox->setStyleSheet("QWidget { background-color: pink; }");
	ui->advancedBox->setStyleSheet("QWidget { background-color: pink; }");
#else
	ui->groupBox->hide();
	ui->advancedBox->hide();
#endif
}

AnovaBayesianForm::~AnovaBayesianForm()
{
	delete ui;
}

void AnovaBayesianForm::bindTo(Options *options, DataSet *dataSet)
{
	AnalysisForm::bindTo(options, dataSet);

	factorsChanging();

	_anovaModel->setVariables(_fixedFactorsListModel->assigned(), _randomFactorsListModel->assigned());

	factorsChanged();
}

void AnovaBayesianForm::factorsChanging()
{
	if (_options != NULL)
		_options->blockSignals(true);
}

void AnovaBayesianForm::factorsChanged()
{
	if (_options != NULL)
		_options->blockSignals(false);
}



