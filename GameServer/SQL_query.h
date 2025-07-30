#pragma once
#include <iostream>
using namespace std;
namespace SQLQuery
{
	string DeleteTableAndReCreate = R"(DROP TABLE IF EXISTS [dbo].[Friend];
					  DROP TABLE IF EXISTS [dbo].[FriendRequest];
					  DROP TABLE IF EXISTS [dbo].[ChatLog];
					  DROP TABLE IF EXISTS [dbo].[User];
					  DROP TABLE IF EXISTS [dbo].[Account];
					  CREATE TABLE [dbo].[Account] 
					  (
						primid INT NOT NULL PRIMARY KEY IDENTITY(1,1),
		                id NVARCHAR(50) NOT NULL,
                        pw NVARCHAR(50) NOT NULL,
						email NVARCHAR(255) NOT NULL
					  );
					  CREATE TABLE [dbo].[User]
					  (
						primid INT NOT NULL,
                        nickname NVARCHAR(50) NOT NULL
					  );
					   CREATE TABLE [dbo].[Friend]
					  (
						user_id INT NOT NULL,
						friend_id INT NOT NULL,
						added_at DATETIME DEFAULT GETDATE(),
						PRIMARY KEY(user_id, friend_id),
					    FOREIGN KEY(user_id) REFERENCES [dbo].[Account](primid),
						FOREIGN KEY(friend_id) REFERENCES [dbo].[Account](primid)
					  );
					   CREATE TABLE [dbo].[FriendRequest] (
				       request_id INT IDENTITY PRIMARY KEY,
				       from_id INT NOT NULL,
				       to_id INT NOT NULL,
				       requested_at DATETIME DEFAULT GETDATE(),
				       status VARCHAR(20) DEFAULT 'pending', -- 'pending', 'accepted', 'declined'
				       FOREIGN KEY(from_id) REFERENCES [dbo].[Account](primid),
				       FOREIGN KEY(to_id) REFERENCES [dbo].[Account](primid)
                    );
					  CREATE TABLE [dbo].[ChatLog] (
					  chat_id INT IDENTITY PRIMARY KEY,
					  sender_id INT NOT NULL,
					  receiver_id INT NOT NULL,
					  message NVARCHAR(MAX) NOT NULL,
					  sent_at DATETIME DEFAULT GETDATE(),
					  FOREIGN KEY(sender_id) REFERENCES [dbo].[Account](primid),
					  FOREIGN KEY(receiver_id) REFERENCES [dbo].[Account](primid)
					);)";


	


}