{
    "targets": [
        {
            "target_name": "ingescape",
            "sources": [ "./src/main.c", "./src/private.c", "./src/admin_config_utils.c",
            "./src/init_control.c", "./src/definition.c", "./src/enum.c", "./src/iop.c",
            "./src/mapping.c", "./src/service.c", "./src/advanced.c", "./src/agent.c"],
            'variables': {
                'openssl_fips' : '0',
                'from_sources%': '0' 
            },
			"conditions": [
                [
                    'OS=="win"',
                    {
                        'defines': [
						  'INGESCAPE_STATIC',
						  'CZMQ_STATIC',
						  'ZMQ_STATIC',
						  'ZYRE_STATIC',
						  'SODIUM_STATIC'
						],
                        "include_dirs": ['C:\\Program Files\\ingescape\\include'],
                        "libraries": [
                            'C:\\Program Files\\ingescape\\lib\\libingescape.lib',
                            'C:\\Program Files\\ingescape\\lib\\libzyre.lib',
                            'C:\\Program Files\\ingescape\\lib\\libczmq.lib',
                            'C:\\Program Files\\ingescape\\lib\\libzmq-v142-mt-s-4_3_5.lib',
                            'C:\\Program Files\\ingescape\\lib\\libsodium.lib',
                            '-lws2_32',
                            '-lIphlpapi',
                            '-lRpcrt4'
                        ],
						'configurations': {
							'Debug': {
								'msvs_settings': {
								
									'VCCLCompilerTool': {
										'RuntimeLibrary': 2
									}
								}
							},
							'Release': {
								'msvs_settings': {
									'VCCLCompilerTool': {
										'RuntimeLibrary': 2
									}
								}
							}
						}
                    }
                ],
                [
                    'OS=="mac"',
                    {
						"include_dirs": ['/usr/local/include/'],
                        "libraries": [
                            "/usr/local/lib/libingescape.a",
                            "/usr/local/lib/libzyre.a",
                            "/usr/local/lib/libczmq.a", 
                            "/usr/local/lib/libzmq.a",
                            "/usr/local/lib/libsodium.a"
                        ]
                    }
                ],
                [
                    'OS=="linux"',
                    {
						"include_dirs": ['/usr/local/include/'],
                        "libraries": [
                            "/usr/local/lib/libingescape.a",
                            "/usr/local/lib/libzyre.a",
                            "/usr/local/lib/libczmq.a", 
                            "/usr/local/lib/libzmq.a",
                            "/usr/local/lib/libsodium.a" 
                        ]
                    }
                ],
                [
                    'from_sources == 1', 
                    {
                        'defines': ['FROM_SOURCES']
                    }
                ]
            ]
        }
    ]
}
