dyn.load(paste("bnnlib", .Platform$dynlib.ext, sep=""))
source("bnnlib.R")
cacheMetaData(1)


# create a network using the Factory class
NetworkFactory_createTestNetwork1(10,10,10,FALSE)
net<-NetworkFactory_createFeedForwardNetwork(10,10,10)

# access properties of a network
Network_in_size_get(net)
