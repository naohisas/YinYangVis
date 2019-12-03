#include "Model.h"
#include <YinYangVis/Lib/Edge.h>
#include <YinYangVis/Lib/YinYangGridSampling.h>
#include <YinYangVis/Lib/ZhongGridSampling.h>
#include <kvs/ExternalFaces>
#include <kvs/Isosurface>
#include <kvs/PolygonToPolygon>
#include <kvs/SmartPointer>
#include <kvs/Indent>
#include "Isosurface.h"


namespace
{

typedef kvs::SharedPointer<kvs::UnstructuredVolumeObject> VolumePointer;

}

namespace local
{

Model::Model( local::Input& input ):
    m_input( input )
{
    std::cout << "IMPORT VOLUMES ..." << std::endl;

    // Import volume dataset.
    this->import_yin_volume();
    this->import_yang_volume();
    this->import_zhong_volume();
    this->update_min_max_values();
    this->update_min_max_coords();

    // Initial value of isovalue.
    const float min_value = m_yin_volume.minValue();
    const float max_value = m_yin_volume.maxValue();
    const float ratio = 0.5f;
    m_isovalue = kvs::Math::Mix( min_value, max_value, ratio );

    // Output information of volume dataset.
    const kvs::Indent indent( 4 );
    m_yin_volume.print( std::cout << "YIN VOLUME DATA" << std::endl, indent );
    m_yang_volume.print( std::cout << "YANG VOLUME DATA" << std::endl, indent );
    m_zhong_volume.print( std::cout << "ZHONG VOLUME DATA" << std::endl, indent );
}

kvs::LineObject* Model::newYinMeshes( const size_t dim_edge ) const
{
    return YinYangVis::Edge::CreateLineMeshObject( &m_yin_volume, dim_edge );
}

kvs::LineObject* Model::newYangMeshes( const size_t dim_edge ) const
{
    return YinYangVis::Edge::CreateLineMeshObject( &m_yang_volume, dim_edge );
}

kvs::LineObject* Model::newYinEdges() const
{
    return YinYangVis::Edge::CreateLineEdgeObject( &m_yin_volume );
}

kvs::LineObject* Model::newYangEdges() const
{
    return YinYangVis::Edge::CreateLineEdgeObject( &m_yang_volume );
}

kvs::LineObject* Model::newZhongEdges() const
{
    return YinYangVis::Edge::CreateLineEdgeObject( &m_zhong_volume );
}

kvs::PolygonObject* Model::newYinFaces() const
{
    ::VolumePointer volume( YinVolume::ToUnstructuredVolumeObject( &m_yin_volume ) );
    return this->newFaces( volume.get() );
}

kvs::PolygonObject* Model::newYangFaces() const
{
    ::VolumePointer volume( YangVolume::ToUnstructuredVolumeObject( &m_yang_volume ) );
    return this->newFaces( volume.get() );
}

kvs::PolygonObject* Model::newZhongFaces() const
{
    ::VolumePointer volume( ZhongVolume::ToUnstructuredVolumeObject( &m_zhong_volume ) );
    return this->newFaces( volume.get() );
}

kvs::PolygonObject* Model::newFaces( const kvs::UnstructuredVolumeObject* volume ) const
{
    return new kvs::ExternalFaces( volume );
}

kvs::PolygonObject* Model::newYinIsosurfaces() const
{
    const double isovalue = m_isovalue;
    const kvs::PolygonObject::NormalType n = kvs::PolygonObject::PolygonNormal;
    const bool d = true;
    const kvs::TransferFunction& tfunc = m_input.tfunc;
    return new YinYangVis::Isosurface( &m_yin_volume, isovalue, n, d, tfunc );

//    ::VolumePointer volume( YinVolume::ToUnstructuredVolumeObject( &m_yin_volume ) );
//    return this->newIsosurfaces( volume.get() );
}

kvs::PolygonObject* Model::newYangIsosurfaces() const
{
    const double isovalue = m_isovalue;
    const kvs::PolygonObject::NormalType n = kvs::PolygonObject::PolygonNormal;
    const bool d = true;
    const kvs::TransferFunction& tfunc = m_input.tfunc;
    return new YinYangVis::Isosurface( &m_yang_volume, isovalue, n, d, tfunc );

//    ::VolumePointer volume( YangVolume::ToUnstructuredVolumeObject( &m_yang_volume ) );
//    return this->newIsosurfaces( volume.get() );
}

kvs::PolygonObject* Model::newZhongIsosurfaces() const
{
    const double isovalue = m_isovalue;
    const kvs::PolygonObject::NormalType n = kvs::PolygonObject::PolygonNormal;
    const bool d = true;
    const kvs::TransferFunction& tfunc = m_input.tfunc;
    return new YinYangVis::Isosurface( &m_zhong_volume, isovalue, n, d, tfunc );

//    ::VolumePointer volume( ZhongVolume::ToUnstructuredVolumeObject( &m_zhong_volume ) );
//    return this->newIsosurfaces( volume.get() );
}

kvs::PolygonObject* Model::newIsosurfaces( const kvs::UnstructuredVolumeObject* volume ) const
{
    const double isovalue = m_isovalue;
    const kvs::PolygonObject::NormalType n = kvs::PolygonObject::PolygonNormal;
    const bool d = true;
    const kvs::TransferFunction& tfunc = m_input.tfunc;
#if defined( JSST2019_TEST )
    kvs::PolygonObject* temp = new kvs::Isosurface( volume, isovalue, n, d, tfunc );
    kvs::PolygonObject* object = new kvs::PolygonToPolygon( temp );
    object->setMinMaxObjectCoords( temp->minObjectCoord(), temp->maxObjectCoord() );
    object->setMinMaxExternalCoords( temp->minExternalCoord(), temp->maxExternalCoord() );
    delete temp;
    return object;
#else
    return new kvs::Isosurface( volume, isovalue, n, d, tfunc );
#endif
}

void Model::import_yin_volume()
{
    const size_t dim_rad = m_input.dim_rad;
    const size_t dim_lat = m_input.dim_lat;
    const size_t dim_lon = m_input.dim_lon;
    const std::string& filename = m_input.filename_yin;

    m_yin_volume.setGridTypeToYin();
    m_yin_volume.setDimR( dim_rad );
    m_yin_volume.setDimTheta( dim_lat );
    m_yin_volume.setDimPhi( dim_lon );
    m_yin_volume.setVeclen( 1 );
    m_yin_volume.calculateCoords();
    m_yin_volume.readValues( filename );
    m_yin_volume.updateMinMaxCoords();
    m_yin_volume.updateMinMaxValues();
}

void Model::import_yang_volume()
{
    const size_t dim_rad = m_input.dim_rad;
    const size_t dim_lat = m_input.dim_lat;
    const size_t dim_lon = m_input.dim_lon;
    const std::string& filename = m_input.filename_yang;

    m_yang_volume.setGridTypeToYang();
    m_yang_volume.setDimR( dim_rad );
    m_yang_volume.setDimTheta( dim_lat );
    m_yang_volume.setDimPhi( dim_lon );
    m_yang_volume.setVeclen( 1 );
    m_yang_volume.calculateCoords();
    m_yang_volume.readValues( filename );
    m_yang_volume.updateMinMaxCoords();
    m_yang_volume.updateMinMaxValues();
}

void Model::import_zhong_volume()
{
    const size_t dim_rad = m_input.dim_rad;
    const size_t dim_zhong = m_input.dim_zhong;
    const std::string& filename = m_input.filename_zhong;

    m_zhong_volume.setDimR( dim_rad );
    m_zhong_volume.setDim( dim_zhong );
    m_zhong_volume.setVeclen( 1 );
    m_zhong_volume.calculateCoords();
    m_zhong_volume.readValues( filename );
    m_zhong_volume.updateMinMaxCoords();
    m_zhong_volume.updateMinMaxValues();
}

void Model::update_min_max_values()
{
    const kvs::Real32 min_value0 = m_yin_volume.minValue();
    const kvs::Real32 min_value1 = m_yang_volume.minValue();
    const kvs::Real32 min_value2 = m_zhong_volume.minValue();
    const kvs::Real32 max_value0 = m_yin_volume.maxValue();
    const kvs::Real32 max_value1 = m_yang_volume.maxValue();
    const kvs::Real32 max_value2 = m_zhong_volume.maxValue();
    const kvs::Real32 min_value = kvs::Math::Min( min_value0, min_value1, min_value2 );
    const kvs::Real32 max_value = kvs::Math::Max( max_value0, max_value1, max_value2 );
    m_yin_volume.setMinMaxValues( min_value, max_value );
    m_yang_volume.setMinMaxValues( min_value, max_value );
    m_zhong_volume.setMinMaxValues( min_value, max_value );
}

void Model::update_min_max_coords()
{
    const kvs::Vec3& min_coord0 = m_yin_volume.minObjectCoord();
    const kvs::Vec3& min_coord1 = m_yang_volume.minObjectCoord();
    const kvs::Vec3& min_coord2 = m_zhong_volume.minObjectCoord();
    const kvs::Vec3& max_coord0 = m_yin_volume.maxObjectCoord();
    const kvs::Vec3& max_coord1 = m_yang_volume.maxObjectCoord();
    const kvs::Vec3& max_coord2 = m_zhong_volume.maxObjectCoord();
    const kvs::Real32 min_x = kvs::Math::Min( min_coord0.x(), min_coord1.x(), min_coord2.x() );
    const kvs::Real32 min_y = kvs::Math::Min( min_coord0.y(), min_coord1.y(), min_coord2.y() );
    const kvs::Real32 min_z = kvs::Math::Min( min_coord0.z(), min_coord1.z(), min_coord2.z() );
    const kvs::Real32 max_x = kvs::Math::Min( max_coord0.x(), max_coord1.x(), max_coord2.x() );
    const kvs::Real32 max_y = kvs::Math::Min( max_coord0.y(), max_coord1.y(), max_coord2.y() );
    const kvs::Real32 max_z = kvs::Math::Min( max_coord0.z(), max_coord1.z(), max_coord2.z() );
    const kvs::Vec3 min_coord( min_x, min_y, min_z );
    const kvs::Vec3 max_coord( max_x, max_y, max_z );
    m_yin_volume.setMinMaxObjectCoords( min_coord, max_coord );
    m_yin_volume.setMinMaxExternalCoords( min_coord, max_coord );
    m_yang_volume.setMinMaxObjectCoords( min_coord, max_coord );
    m_yang_volume.setMinMaxExternalCoords( min_coord, max_coord );
    m_zhong_volume.setMinMaxObjectCoords( min_coord, max_coord );
    m_zhong_volume.setMinMaxExternalCoords( min_coord, max_coord );
}

} // end of namespace local
