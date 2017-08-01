import sys, string
import alignment
from pyspark.sql.functions import substring
from pyspark.sql import SparkSession
#create spark session
spark = SparkSession\
.builder\
.appName("Needleman")\
.getOrCreate()
#get sequences from files
#peform spark transformation
seq1 = spark.read.text("hdfs://HadoopMaster:9000/user/hduser/seq1.txt").collect()
seq2 = spark.read.text("hdfs://HadoopMaster:9000/user/hduser/seq2.txt").collect()
#perform spark action
for i in range(0,1):
    alignment.needle(seq1[i].value,seq2[i].value)
