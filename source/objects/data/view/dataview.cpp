#include "objects/data/view/dataview.h"

#include "common/exception.h"

using namespace love;

love::Type DataView::type("DataView", &Data::type);

DataView::DataView(Data* data, size_t offset, size_t size) : data(data), offset(offset), size(size)
{
    if (offset >= data->GetSize() || size > data->GetSize() || offset > data->GetSize() - size)
        throw love::Exception(
            "Offset and size of DataView must fit within the original Data's size!");

    if (size == 0)
        throw love::Exception("DataView size must be grater than zero!");
}

DataView::DataView(const DataView& other) : data(other.data), offset(other.offset), size(other.size)
{}

DataView::~DataView()
{}

DataView* DataView::Clone() const
{
    return new DataView(*this);
}

void* DataView::GetData() const
{
    return (uint8_t*)this->data->GetData() + offset;
}

size_t DataView::GetSize() const
{
    return this->size;
}
