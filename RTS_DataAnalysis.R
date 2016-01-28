#Photon Remote Temperature Sensor Data Plot and Analysis
#Code written by Jennifer Fox <jenfoxbot@gmail.com> 


tempPlot = function(file = ''){
	default = "C:\\test\\*.txt"

	#Load in CSV text file 
	if (file == '') file = file.choose()

	#Set data into matrix
	headers = c("Temp_C", "Unit_C", "Timestamp")
	temp_raw = read.table(file, sep = ",", quote = "", skip = 1, fill = TRUE, col.names = headers)
	
	#Add in degrees Fahrenheit 
	tempF = matrix(nrow = length(temp_raw[,1]), ncol = 2, byrow = TRUE)
	tempF[,1] = temp_raw[,1]*1.8+32
	tempF[,2] = "F"	
	temp_raw[,c("Temp_F","Unit_F")] = c(as.numeric(tempF[1:length(tempF[,1]),1]), tempF[,2])
	temp = temp_raw[,c(1,2,4,5,3)]	

	#Convert UTC timestamp into local (sensor) timezone
	timestamp_raw = temp$Timestamp
	ts_placeholder = gsub("T", " ", timestamp_raw)
	timestamp = gsub("Z", "", ts_placeholder)
	my_timezone = as.POSIXlt(timestamp, origin="1970-01-01") 

	#Replace temp data with local timezone 
	temp$Timestamp = my_timezone


	#Plot temperature data
	plot(temp$Timestamp, temp$Temp_C, main = "Temperature (°C) vs. Time", ylab = "Temperature (°C)", xlab = "Time (hours)", xaxt = "n")
	axis.POSIXct(1, my_timezone, at = , labels = TRUE, format = "%H:00") #Adjust or remove the format variable to change the time axis
	dev.new()
	plot(temp$Timestamp, temp$Temp_F, main = "Temperature (°F) vs. Time", ylab = "Temperature (°F)", xlab = "Time (hours)", xaxt = "n")
	axis.POSIXct(1, my_timezone, labels = TRUE, format = "%H:00")


	#Calculate and output basic statistical analysis
	end_date = temp$Timestamp[1]
	start_date = temp$Timestamp[length(temp$Timestamp)]
	t_F = summary(as.numeric(temp$Temp_F))
	t_C = summary(as.numeric(temp$Temp_C))

	cat("Data Stream Start Date:", as.character(start_date), "\n", "Data Stream End Date:", as.character(end_date), "\n\n")
	cat("Summary of Temperature Data (°C)", "\n", "Average (Mean) Temperature: ", t_C["Mean"] ,"\n", "Minimum Temp: ", t_C["Min."], "\n", "Maximum Temp: ", t_C["Max."], "\n")
	cat("Summary of Temperature Data (°F)", "\n", "Average (Mean) Temp: ", t_F["Mean"] ,"\n", "Minimum Temp: ", t_F["Min."], "\n", "Maximum Temp: ", t_F["Max."], "\n")

}

