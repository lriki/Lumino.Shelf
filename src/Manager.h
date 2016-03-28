#pragma once

class Manager
{
public:
	void SetSourceDirectory(const PathName& path) { m_sourceDirectory = path; }
	void SetTemplateDirectory(const PathName& path) { m_templateDirectory = path; }
	void SetReleaseDirectory(const PathName& path) { m_releaseDirectory = path; }
	void Build();

private:
	PathName	m_sourceDirectory;
	PathName	m_templateDirectory;
	PathName	m_releaseDirectory;
};
