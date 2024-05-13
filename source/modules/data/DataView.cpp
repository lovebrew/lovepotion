#include "common/Exception.hpp"
#include "common/int.hpp"

#include "modules/data/DataView.hpp"

namespace love
{
    Type DataView::type("DataView", &Data::type);

    DataView::DataView(Data* data, size_t offset, size_t size) : data(data), offset(offset), size(size)
    {
        if (offset >= data->getSize() || size > data->getSize() || offset > data->getSize() - size)
            throw love::Exception(E_DATAVIEW_OFFSET_AND_SIZE);

        if (size == 0)
            throw love::Exception(E_DATAVIEW_INVALID_SIZE);
    }

    DataView::DataView(const DataView& other) : data(other.data), offset(other.offset), size(other.size)
    {}

    DataView::~DataView()
    {}

    DataView* DataView::clone() const
    {
        return new DataView(*this);
    }

    void* DataView::getData() const
    {
        return (uint8_t*)this->data->getData() + this->offset;
    }

    size_t DataView::getSize() const
    {
        return this->size;
    }
} // namespace love
