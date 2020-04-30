#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/ColorMap>
#include <kvs/KeyPressEventListener>
#include <YYZVis/Lib/YinVolumeImporter.h>
#include <YYZVis/Lib/YangVolumeImporter.h>
#include <YYZVis/Lib/ZhongVolumeImporter.h>
#include <YYZVis/Lib/UpdateMinMaxValues.h>
#include <YYZVis/Lib/SlicePlane.h>


class KeyPressEvent : public kvs::KeyPressEventListener
{
    void update( kvs::KeyEvent* event )
    {
        switch ( event->key() )
        {
        case kvs::Key::One:
        {
            auto* object = scene()->object( "Yin" );
            if ( object->isShown() ) { object->hide(); }
            else { object->show(); }
            break;
        }
        case kvs::Key::Two:
        {
            auto* object = scene()->object( "Yang" );
            if ( object->isShown() ) { object->hide(); }
            else { object->show(); }
            break;
        }
        case kvs::Key::Three:
        {
            auto* object = scene()->object( "Zhong" );
            if ( object->isShown() ) { object->hide(); }
            else { object->show(); }
            break;
        }
        default: break;
        }
    }
};

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.setTitle( "YYZVis::SlicePlane" );
    screen.setBackgroundColor( kvs::RGBColor::White() );

    // Import YYZ data.
    const std::string input_file( argv[1] );
    auto* yin_volume = new YYZVis::YinVolumeImporter( input_file );
    auto* yng_volume = new YYZVis::YangVolumeImporter( input_file );
    auto* zng_volume = new YYZVis::ZhongVolumeImporter( input_file );
    YYZVis::UpdateMinMaxValues( yin_volume, yng_volume, zng_volume );

    // Dump.
    const kvs::Indent indent( 4 );
    yin_volume->print( std::cout << "YIN VOLUME DATA" << std::endl, indent );
    yng_volume->print( std::cout << "YANG VOLUME DATA" << std::endl, indent );
    zng_volume->print( std::cout << "ZHONG VOLUME DATA" << std::endl, indent );

    // Extract slice planes.
    const kvs::Vec3 point( 0.0f, 0.0f, 0.0f );
    const kvs::Vec3 normal( 0.0f, 0.0f, 1.0f );
    const kvs::ColorMap cmap = kvs::ColorMap::BrewerSpectral();
    auto* yin_object = new YYZVis::SlicePlane( yin_volume, point, normal, cmap );
    auto* yng_object = new YYZVis::SlicePlane( yng_volume, point, normal, cmap );
    auto* zng_object = new YYZVis::SlicePlane( zng_volume, point, normal, cmap );
    delete yin_volume;
    delete yng_volume;
    delete zng_volume;

    yin_object->setName( "Yin" );
    yng_object->setName( "Yang" );
    zng_object->setName( "Zhong" );

    screen.registerObject( yin_object );
    screen.registerObject( yng_object );
    screen.registerObject( zng_object );

    // Key press event.
    KeyPressEvent key_event;
    screen.addEvent( &key_event );

    screen.show();

    return app.run();
}
