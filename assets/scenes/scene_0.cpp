#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/ws.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace rl { namespace scene {

    void scene_0( ptr_t<Scene> self ) {

        struct NODE {
            string_t Message = string_t( UNBFF_SIZE, '\0' );
            string_t Response, FullMessage;
        };  ptr_t<NODE> obj = new NODE();

    /*.........................................................................*/

        event_t<string_t> onGPT; onGPT.on([=]( string_t message ){
            
            fetch_t args; ssl_t ssl; args.method = "POST";
                    args.url    = regex::format("${0}?key=${1}",process::env::get("URL"),process::env::get("API"));
                    
                    args.headers= header_t({
                        { "Content-Type", path::mimetype(".json") },
                        { "Host", url::hostname( args.url ) }
                    });

                    args.body   = json::stringify( object_t({
                        { "contents", array_t<object_t>({ object_t({
                            { "parts", array_t<object_t>({ object_t({
                                { "text", message }
                            }) }) }
                        }) }) }
                    }) );

            https::fetch( args, &ssl )

            .then([=]( https_t cli ){
                auto req = json::parse( regex::replace_all( stream::await( cli ), "\\\\\"|*", "" ) );
                auto data= req["candidates"][0]["content"]["parts"][0]["text"].as<string_t>();
                     data= regex::replace_all( data, "\\\\n", "\n" ); 
                     obj->FullMessage = data; obj->Response.clear();
            })

            .fail([=]( except_t err ){
                console::log( err );
            });

        });

    /*.........................................................................*/

        self->onDraw([=](){

            float w = GetRenderWidth(), h = GetRenderHeight(); ClearBackground( RAYWHITE );

            GuiDrawText( obj->Response.get(), { 2*w/100, 2*h/100, 95*w/100, 95*h/100 }, 0, BLACK ); 

            auto x = GuiTextBox( { 0, 95*h/100, 88*w/100, 5*h/100 }, obj->Message.get(), obj->Message.size(), true );

            if( GuiButton( { 88*w/100, 95*h/100, 12*w/100, 5*h/100 }, "Send" ) ){
                onGPT.emit( obj->Message.copy() );  obj->Message.fill( '\0' );
            }

        });

    /*.........................................................................*/

        self->onLoop([=]( float delta ){ [=](){
        coStart

            while( obj->Response != obj->FullMessage ){
                obj->Response.push( obj->FullMessage[ obj->Response.size() ] );
                coNext;
            }
            
        coStop
        }(); });

    }

}}

/*────────────────────────────────────────────────────────────────────────────*/