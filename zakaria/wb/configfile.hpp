/*
	same port -> error
	parse the config file

	store the configs one by one in a map< host:port, server &> to make sure of
				{The first server for a host:port will be the default for this host:port}
	then return a vector of <server & >
*/
