#pragma once

#include <QAbstractItemModel>
#include <QString>
#include <QModelIndex>
#include <QVariant>


namespace OpenInfraPlatform {
	namespace UserInterface {
		class TreeItem;

		class TreeModel : public QAbstractItemModel {
			Q_OBJECT;

		public:
			TreeModel(std::map<int, shared_ptr<OpenInfraPlatform::IfcAlignment1x1::IfcAlignment1x1Entity>> &data, QObject *parent = 0);
			//explicit TreeModel(const QString &data, QObject *parent = 0);
			~TreeModel();

			QVariant data(const QModelIndex &index, int role) const override;

			Qt::ItemFlags flags(const QModelIndex &index) const override;

			QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

			QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

			QModelIndex parent(const QModelIndex &index) const override;

			int rowCount(const QModelIndex &parent = QModelIndex()) const override;

			int columnCount(const QModelIndex &parent = QModelIndex()) const override;

		private:
			//void setupModelData(const QStringList &lines, TreeItem *parent);

			TreeItem *rootItem;
			//std::vector<shared_ptr<TreeItem>> data_;
		};
	}
}
